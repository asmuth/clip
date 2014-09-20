/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <assert.h>
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/runtime/queryplanbuilder.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/sql/runtime/tablelessselect.h>
#include <fnordmetric/sql/runtime/tablescan.h>
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <fnordmetric/sql/runtime/limitclause.h>
#include <fnordmetric/sql/runtime/groupby.h>
#include <fnordmetric/sql/runtime/runtime.h>
#include <fnordmetric/sql/runtime/symboltable.h>
#include <fnordmetric/sql/runtime/importstatement.h>

namespace fnordmetric {
namespace query {

QueryPlanBuilder::QueryPlanBuilder(
    Compiler* compiler,
    const std::vector<std::unique_ptr<Backend>>& backends) :
    QueryPlanBuilderInterface(compiler, backends) {}

void QueryPlanBuilder::buildQueryPlan(
    const std::vector<std::unique_ptr<ASTNode>>& statements,
    QueryPlan* query_plan) {

  for (const auto& stmt : statements) {
    switch (stmt->getType()) {
      case query::ASTNode::T_SELECT: {
        auto query_plan_node = buildQueryPlan(
            stmt.get(),
            query_plan->tableRepository());

        if (query_plan_node == nullptr) {
          RAISE(
              util::RuntimeException,
              "can't figure out a query plan for this, sorry :(");
        }

        query_plan->addQuery(std::unique_ptr<QueryPlanNode>(query_plan_node));
        break;
      }

      case query::ASTNode::T_IMPORT:
        query_plan->tableRepository()->import(
            ImportStatement(stmt.get(), compiler_),
            backends_);
        break;

      default:
        RAISE(util::RuntimeException, "invalid statement");
    }
  }
}

QueryPlanNode* QueryPlanBuilder::buildQueryPlan(
    ASTNode* ast,
    TableRepository* repo) {
  QueryPlanNode* exec = nullptr;

  for (const auto& extension : extensions_) {
    exec = extension->buildQueryPlan(ast, repo);

    if (exec != nullptr) {
      return exec;
    }
  }

  /* internal nodes: multi table query (joins), order, aggregation, limit */
  if ((exec = buildLimitClause(ast, repo)) != nullptr) {
    return exec;
  }

  if ((exec = buildOrderByClause(ast, repo)) != nullptr) {
    return exec;
  }

  if (hasGroupByClause(ast) || hasAggregationInSelectList(ast)) {
    return buildGroupBy(ast, repo);
  }

  /* leaf nodes: table scan, tableless select */
  if ((exec = TableScan::build(ast, repo, compiler_)) != nullptr) {
    return exec;
  }

  if ((exec = TablelessSelect::build(ast, compiler_)) != nullptr) {
    return exec;
  }

  RAISE(
      util::RuntimeException,
      "can't figure out a query plan for this, sorry :(");

  // if verbose -> dump ast
  return nullptr;
}

bool QueryPlanBuilder::hasGroupByClause(ASTNode* ast) const {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 2) {
    return false;
  }

  for (const auto& child : ast->getChildren()) {
    if (child->getType() == ASTNode::T_GROUP_BY) {
      return true;
    }
  }

  return false;
}

bool QueryPlanBuilder::hasAggregationInSelectList(ASTNode* ast) const {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 2) {
    return false;
  }

  auto select_list = ast->getChildren()[0];
  assert(select_list->getType() == ASTNode::T_SELECT_LIST);

  return hasAggregationExpression(select_list);
}

bool QueryPlanBuilder::hasAggregationExpression(ASTNode* ast) const {
  if (ast->getType() == ASTNode::T_METHOD_CALL) {
    assert(ast->getToken() != nullptr);
    auto symbol = compiler_->symbolTable()->lookupSymbol
        (ast->getToken()->getString());

    if(symbol != nullptr); // FIXPAUL!!!!
    if (symbol->isAggregate()) {
      return true;
    }
  }

  for (const auto& child : ast->getChildren()) {
    if (hasAggregationExpression(child)) {
      return true;
    }
  }

  return false;
}

QueryPlanNode* QueryPlanBuilder::buildGroupBy(
    ASTNode* ast,
    TableRepository* repo) {
  ASTNode group_exprs(ASTNode::T_GROUP_BY);
  assert(ast->getChildren()[0]->getType() == ASTNode::T_SELECT_LIST);

  /* copy own select list */
  auto select_list = ast->getChildren()[0]->deepCopy();

  /* generate select list for child */
  auto child_sl = new ASTNode(ASTNode::T_SELECT_LIST);
  buildInternalSelectList(select_list, child_sl);

  /* copy ast for child and swap out select lists*/
  auto child_ast = ast->deepCopy();
  child_ast->removeChild(0);
  child_ast->appendChild(child_sl, 0);

  /* search for a group by clause */
  for (const auto& child : ast->getChildren()) {
    if (child->getType() != ASTNode::T_GROUP_BY) {
      continue;
    }

    /* FIXPAUL resolve aliases in group list from select list, return error
       if alias contains aggregate func */

    /* copy all group expressions and add required field to child select list */
    for (const auto& group_expr : child->getChildren()) {
      auto e = group_expr->deepCopy();
      buildInternalSelectList(e, child_sl);
      group_exprs.appendChild(e);
    }

    /* remove group by clause from child ast */
    const auto& child_ast_children = child_ast->getChildren();
    for (int i = 0; i < child_ast_children.size(); ++i) {
      if (child_ast_children[i]->getType() == ASTNode::T_GROUP_BY) {
        child_ast->removeChild(i);
        break;
      }
    }
  }

  /* compile select list and group expressions */
  size_t select_scratchpad_len = 0;
  auto select_expr = compiler_->compile(select_list, &select_scratchpad_len);

  size_t group_scratchpad_len = 0;
  auto group_expr = compiler_->compile(&group_exprs, &group_scratchpad_len);
  assert(group_scratchpad_len == 0);
  //child_ast->debugPrint(2);
  //select_list->debugPrint(2);

  /* resolve output column names */
  std::vector<std::string> column_names;
  for (const auto& col : select_list->getChildren()) {
    column_names.push_back("unnamed");
  }

  return new GroupBy(
      std::move(column_names),
      select_expr,
      group_expr,
      select_scratchpad_len,
      buildQueryPlan(child_ast, repo));
}


bool QueryPlanBuilder::buildInternalSelectList(
    ASTNode* node,
    ASTNode* target_select_list) {
  /* search for column references recursively */
  if (node->getType() == ASTNode::T_COLUMN_NAME) {
    auto col_index = -1;

    /* check if this columns is already in the select list */
    const auto& candidates = target_select_list->getChildren();
    for (int i = 0; i < candidates.size(); ++i) {
      // FIXPAUL
    }

    /* otherwise add this column to the select list */
    if (col_index < 0) {
      auto derived = new ASTNode(ASTNode::T_DERIVED_COLUMN);
      derived->appendChild(node->deepCopy());
      target_select_list->appendChild(derived);
      col_index = target_select_list->getChildren().size() - 1;
    }

    node->setType(ASTNode::T_RESOLVED_COLUMN);
    node->setID(col_index);
    return true;
  } else {
    for (const auto& child : node->getChildren()) {
      if (!buildInternalSelectList(child, target_select_list)) {
        return false;
      }
    }

    return true;
  }
}

QueryPlanNode* QueryPlanBuilder::buildLimitClause(
    ASTNode* ast,
    TableRepository* repo) {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 3) {
    return nullptr;
  }

  for (const auto& child : ast->getChildren()) {
    int limit = 0;
    int offset = 0;

    if (child->getType() != ASTNode::T_LIMIT) {
      continue;
    }

    auto limit_token = child->getToken();
    assert(limit_token);
    assert(*limit_token == Token::T_NUMERIC);
    limit = limit_token->getInteger();

    if (child->getChildren().size() == 1) {
      assert(child->getChildren()[0]->getType() == ASTNode::T_OFFSET);
      auto offset_token = child->getChildren()[0]->getToken();
      assert(offset_token);
      assert(*offset_token == Token::T_NUMERIC);
      offset = offset_token->getInteger();
    }

    auto new_ast = ast->deepCopy();
    const auto& new_ast_children = new_ast->getChildren();

    for (int i = 0; i < new_ast_children.size(); ++i) {
      if (new_ast_children[i]->getType() == ASTNode::T_LIMIT) {
        new_ast->removeChild(i);
        break;
      }
    }

    return new LimitClause(limit, offset, buildQueryPlan(new_ast, repo));
  }

  return nullptr;
}

QueryPlanNode* QueryPlanBuilder::buildOrderByClause(
    ASTNode* ast,
    TableRepository* repo) {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 3) {
    return nullptr;
  }

  for (const auto& child : ast->getChildren()) {
    if (child->getType() != ASTNode::T_ORDER_BY) {
      continue;
    }

    RAISE(util::RuntimeException, "ORDER BY not yet implemented");
  }

  return nullptr;
}


void QueryPlanBuilder::extend(
    std::unique_ptr<QueryPlanBuilderInterface> other) {
  extensions_.emplace_back(std::move(other));
}

}
}
