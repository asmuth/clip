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
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/runtime/queryplanbuilder.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/sql/runtime/tablelessselect.h>
#include <fnordmetric/sql/runtime/tablescan.h>
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <fnordmetric/sql/runtime/limitclause.h>
#include <fnordmetric/sql/runtime/orderby.h>
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
              kRuntimeError,
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
        RAISE(kRuntimeError, "invalid statement");
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

  RAISE(kRuntimeError, "can't figure out a query plan for this, sorry :(");

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
  if (!(select_list->getType() == ASTNode::T_SELECT_LIST)) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  return hasAggregationExpression(select_list);
}

bool QueryPlanBuilder::hasAggregationExpression(ASTNode* ast) const {
  if (ast->getType() == ASTNode::T_METHOD_CALL) {
    if (!(ast->getToken() != nullptr)) {
      RAISE(kRuntimeError, "corrupt AST");
    }

    auto symbol = compiler_->symbolTable()->lookupSymbol
        (ast->getToken()->getString());

    if (symbol == nullptr) {
      RAISE(kRuntimeError, "symbol lookup failed");
    }

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
  if (!(ast->getChildren()[0]->getType() == ASTNode::T_SELECT_LIST)) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  /* copy own select list */
  auto select_list = ast->getChildren()[0]->deepCopy();

  /* generate select list for child */
  auto child_sl = new ASTNode(ASTNode::T_SELECT_LIST);
  buildInternalSelectList(select_list, child_sl);

  /* copy ast for child and swap out select lists*/
  auto child_ast = ast->deepCopy();
  child_ast->removeChildByIndex(0);
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
        child_ast->removeChildByIndex(i);
        break;
      }
    }
  }

  /* compile select list and group expressions */
  size_t select_scratchpad_len = 0;
  auto select_expr = compiler_->compile(select_list, &select_scratchpad_len);

  size_t group_scratchpad_len = 0;
  auto group_expr = compiler_->compile(&group_exprs, &group_scratchpad_len);

  if (group_scratchpad_len > 0) {
    RAISE(kRuntimeError, "GROUP clause can only contain pure functions");
  }

  /* resolve output column names */
  std::vector<std::string> column_names;
  for (const auto& col : select_list->getChildren()) {
    column_names.push_back("unnamed"); // FIXPAUL
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
    if (!(limit_token)) {
      RAISE(kRuntimeError, "corrupt AST");
    }

    if (!(*limit_token == Token::T_NUMERIC)) {
      RAISE(kRuntimeError, "corrupt AST");
    }

    limit = limit_token->getInteger();

    if (child->getChildren().size() == 1) {
      if (!(child->getChildren()[0]->getType() == ASTNode::T_OFFSET)) {
        RAISE(kRuntimeError, "corrupt AST");
      }

      auto offset_token = child->getChildren()[0]->getToken();
      if (!(offset_token)) {
        RAISE(kRuntimeError, "corrupt AST");
      }

      if (!(*offset_token == Token::T_NUMERIC)) {
        RAISE(kRuntimeError, "corrupt AST");
      }
      offset = offset_token->getInteger();
    }

    // clone ast + remove limit clause
    auto new_ast = ast->deepCopy();
    new_ast->removeChildrenByType(ASTNode::T_LIMIT);

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

    // build sort spec
    std::vector<OrderBy::SortSpec> sort_specs;

    // clone ast + remove order by clause
    auto new_ast = ast->deepCopy();
    new_ast->removeChildrenByType(ASTNode::T_ORDER_BY);

    return new OrderBy(sort_specs, buildQueryPlan(new_ast, repo));
  }

  return nullptr;
}


void QueryPlanBuilder::extend(
    std::unique_ptr<QueryPlanBuilderInterface> other) {
  extensions_.emplace_back(std::move(other));
}

}
}
