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
#include <fnordmetric/sql/parser/astutil.h>
#include <fnordmetric/sql/runtime/queryplanbuilder.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/sql/runtime/tablelessselect.h>
#include <fnordmetric/sql/runtime/tablescan.h>
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <fnordmetric/sql/runtime/limitclause.h>
#include <fnordmetric/sql/runtime/orderby.h>
#include <fnordmetric/sql/runtime/groupby.h>
#include <fnordmetric/sql/runtime/groupovertimewindow.h>
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

  /* exapand all column names + wildcard to tablename->columnanme */
  if (hasUnexpandedColumns(ast)) {
    expandColumns(ast, repo);
  }

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

  if (hasOrderByClause(ast)) {
    return buildOrderByClause(ast, repo);
  }

  // FIXPAUL move to sql extensions
  if (hasGroupOverTimewindowClause(ast)) {
    return buildGroupOverTimewindow(ast, repo);
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

bool QueryPlanBuilder::hasUnexpandedColumns(ASTNode* ast) const {
  if (ast->getType() != ASTNode::T_SELECT) {
    return false;
  }

  if (ast->getChildren().size() < 1 ||
      ast->getChildren()[0]->getType() != ASTNode::T_SELECT_LIST) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  if (ast->getChildren().size() == 1) {
    return false;
  }

  for (const auto& col : ast->getChildren()[0]->getChildren()) {
    if (col->getType() != ASTNode::T_DERIVED_COLUMN ||
        col->getChildren().size() != 1 ||
        col->getChildren()[0]->getType() != ASTNode::T_TABLE_NAME ||
        col->getChildren()[0]->getChildren().size() != 1 ||
        col->getChildren()[0]->getChildren()[0]->getType() !=
        ASTNode::T_COLUMN_NAME) {
      return true;
    }
  }

  return false;
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

bool QueryPlanBuilder::hasGroupOverTimewindowClause(ASTNode* ast) const {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 2) {
    return false;
  }

  for (const auto& child : ast->getChildren()) {
    if (child->getType() == ASTNode::T_GROUP_OVER_TIMEWINDOW) {
      return true;
    }
  }

  return false;
}


bool QueryPlanBuilder::hasJoin(ASTNode* ast) const {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 2) {
    return false;
  }

  auto from_list = ast->getChildren()[1];
  if (from_list->getType() != ASTNode::T_FROM ||
      from_list->getChildren().size() < 1) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  if (from_list->getChildren().size() > 1) {
    return true;
  }

  for (const auto& child : ast->getChildren()) {
    //if (child->getType() == ASTNode::T_JOIN) {
    //  return true;
    //}
  }

  return false;
}



bool QueryPlanBuilder::hasOrderByClause(ASTNode* ast) const {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 2) {
    return false;
  }

  for (const auto& child : ast->getChildren()) {
    if (child->getType() == ASTNode::T_ORDER_BY) {
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

void QueryPlanBuilder::expandColumns(ASTNode* ast, TableRepository* repo) {
  if (ast->getChildren().size() < 2) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  auto select_list = ast->getChildren()[0];
  if (select_list->getType() != ASTNode::T_SELECT_LIST) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  auto from_list = ast->getChildren()[1];
  if (from_list->getType() != ASTNode::T_FROM ||
      from_list->getChildren().size() < 1) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  for (const auto& node : select_list->getChildren()) {
    /* expand wildcard select (SELECT * FROM ...) */
    if (node->getType() == ASTNode::T_ALL) {
      if (hasJoin(ast) || from_list->getChildren().size() != 1) {
        RAISE(
            kRuntimeError,
            "can't use wilcard select (SELECT * FROM ...) when selecting from "
            "multiple tables");
      }

      auto table_name = from_list->getChildren()[0];
      if (table_name->getType() != ASTNode::T_TABLE_NAME ||
          table_name->getToken() == nullptr) {
        RAISE(kRuntimeError, "corrupt AST");
      }

      select_list->removeChild(node);

      auto tbl_ref = repo->getTableRef(table_name->getToken()->getString());
      if (tbl_ref == nullptr) {
        RAISE(
            kRuntimeError,
            "unknown table: %s",
            table_name->getToken()->getString().c_str());
      }

      for (const auto& column : tbl_ref->columns()) {
        auto derived_col = new ASTNode(ASTNode::T_DERIVED_COLUMN);
        auto derived_table_name = new ASTNode(ASTNode::T_TABLE_NAME);
        derived_table_name->setToken(table_name->getToken());
        auto column_name = new ASTNode(ASTNode::T_COLUMN_NAME);
        column_name->setToken(new Token(Token::T_IDENTIFIER, column));
        derived_col->appendChild(derived_table_name);
        derived_table_name->appendChild(column_name);
        select_list->appendChild(derived_col);
      }

      continue;
    }
  }

}

QueryPlanNode* QueryPlanBuilder::buildGroupBy(
    ASTNode* ast,
    TableRepository* repo) {
  ASTNode group_exprs(ASTNode::T_GROUP_BY);

  /* copy own select list */
  if (!(ast->getChildren()[0]->getType() == ASTNode::T_SELECT_LIST)) {
    RAISE(kRuntimeError, "corrupt AST");
  }

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
    child_ast->removeChildrenByType(ASTNode::T_GROUP_BY);
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
  auto column_names = ASTUtil::columnNamesFromSelectList(select_list);

  return new GroupBy(
      std::move(column_names),
      select_expr,
      group_expr,
      select_scratchpad_len,
      buildQueryPlan(child_ast, repo));
}

QueryPlanNode* QueryPlanBuilder::buildGroupOverTimewindow(
    ASTNode* ast,
    TableRepository* repo) {
  ASTNode group_exprs(ASTNode::T_GROUP_BY);
  ASTNode* time_expr_ast;
  ASTNode* window_expr_ast;
  ASTNode* step_expr_ast = nullptr;

  /* copy own select list */
  if (!(ast->getChildren()[0]->getType() == ASTNode::T_SELECT_LIST)) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  auto select_list = ast->getChildren()[0]->deepCopy();

  /* generate select list for child */
  auto child_sl = new ASTNode(ASTNode::T_SELECT_LIST);
  buildInternalSelectList(select_list, child_sl);

  /* copy ast for child and swap out select lists*/
  auto child_ast = ast->deepCopy();
  child_ast->removeChildByIndex(0);
  child_ast->appendChild(child_sl, 0);

  /* search for a group over timewindow clause */
  for (const auto& child : ast->getChildren()) {
    if (child->getType() != ASTNode::T_GROUP_OVER_TIMEWINDOW) {
      continue;
    }

    if (child->getChildren().size() < 3) {
      RAISE(kRuntimeError, "corrupt AST");
    }

    /* FIXPAUL resolve aliases in group list from select list, return error
       if alias contains aggregate func */

    /* copy time expression and add required fields to the child select list */
    time_expr_ast = child->getChildren()[0]->deepCopy();
    buildInternalSelectList(time_expr_ast, child_sl);

    /* copy all group exprs and add required fields to the child select list */
    auto group_by_list = child->getChildren()[1];
    for (const auto& group_expr : group_by_list->getChildren()) {
      auto e = group_expr->deepCopy();
      buildInternalSelectList(e, child_sl);
      group_exprs.appendChild(e);
    }

    /* copy window and step expressions */
    window_expr_ast = child->getChildren()[2]->deepCopy();
    if (child->getChildren().size() > 3) {
      step_expr_ast = child->getChildren()[3]->deepCopy();
    }

    /* remove group by clause from child ast */
    child_ast->removeChildrenByType(ASTNode::T_GROUP_OVER_TIMEWINDOW);
  }

  /* compile select list and group expressions */
  size_t select_scratchpad_len = 0;
  auto select_expr = compiler_->compile(select_list, &select_scratchpad_len);

  size_t group_scratchpad_len = 0;
  auto group_expr = compiler_->compile(&group_exprs, &group_scratchpad_len);

  if (group_scratchpad_len > 0) {
    RAISE(
        kRuntimeError,
        "illegal use of aggregate functions in the GROUP BY clause");
  }

  /* compile time expression */
  size_t time_expr_scratchpad_len = 0;
  auto time_expr = compiler_->compile(
      time_expr_ast,
      &time_expr_scratchpad_len);

  if (time_expr_scratchpad_len > 0) {
    RAISE(
        kRuntimeError,
        "illegal use of aggregate functions in the GROUP OVER clause");
  }

  /* find time expression input column */
  if (time_expr_ast->getType() != ASTNode::T_RESOLVED_COLUMN) {
    RAISE(
        kRuntimeError,
        "first argument to TIMEWINDOW() must be a column reference");
  }

  auto input_row_size = child_sl->getChildren().size();
  auto input_row_time_index = time_expr_ast->getID();

  /* compile window and step */
  auto window_svalue = executeSimpleConstExpression(compiler_, window_expr_ast);
  auto window = window_svalue.getInteger();

  fnordmetric::IntegerType step;
  if (step_expr_ast == nullptr) {
    step = window;
  } else {
    auto step_svalue = executeSimpleConstExpression(compiler_, step_expr_ast);
    step = step_svalue.getInteger();
  }

  /* resolve output column names */
  auto column_names = ASTUtil::columnNamesFromSelectList(select_list);

  return new GroupOverTimewindow(
      std::move(column_names),
      time_expr,
      window,
      step,
      input_row_size,
      input_row_time_index,
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

    /* check if this column already exists in the select list */
    const auto& candidates = target_select_list->getChildren();
    for (int i = 0; i < candidates.size(); ++i) {
      if (candidates[i]->getType() == ASTNode::T_DERIVED_COLUMN) {
        if (candidates[i]->getChildren().size() == 1) {
          auto colname = candidates[i]->getChildren()[0];
          if (colname->getType() == ASTNode::T_COLUMN_NAME &&
              node->getToken()->getString() ==
                  colname->getToken()->getString()) {
            col_index = i;
            break;
          }
        }
      }
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
  std::vector<OrderBy::SortSpec> sort_specs;

  /* copy select list for child */
  if (!(ast->getChildren()[0]->getType() == ASTNode::T_SELECT_LIST)) {
    RAISE(kRuntimeError, "corrupt AST");
  }
  auto child_sl = ast->getChildren()[0]->deepCopy();

  /* search for the order by clause */
  for (const auto& child : ast->getChildren()) {
    if (child->getType() != ASTNode::T_ORDER_BY) {
      continue;
    }

    /* build each sort spec and expand select list for missing columns */
    auto sort_specs_asts = child->getChildren();
    for (int i = 0; i < sort_specs_asts.size(); ++i) {
      auto sort = sort_specs_asts[i];
      //if (sort->getChildren().size() != 1 ||
      //    sort->getChildren()[0]->getType() != ASTNode::T_COLUMN_NAME) {
      //  RAISE(kRuntimeError, "corrupt AST");
      //}

      auto col = sort->getChildren()[0];
      size_t col_index;
      bool col_found = false;

      /* check if column is already included in the child select list */
      for (const auto& derived : child_sl->getChildren()) {
        /*
        if (derived->getChildren().size != 1 ||
            derived->getChildren()[0]->getType() != ASTNode::T_DERVIED_COLUMN) {
          RAISE(kRuntimeError, "corrupt AST");
        }

        auto sel_col = derived.getChildren()[0];
        if (col->getToken()->toString() == sel_col->getToken()->toString()) {
          col_index = i;
          col_found = true;
          break;
        }
        */
      }

      /* otherwise add the column to the child select list */
      if (!col_found) {
        auto new_derived = new ASTNode(ASTNode::T_DERIVED_COLUMN);
        new_derived->appendChild(col);
        child_sl->appendChild(new_derived);
        col_index = child_sl->getChildren().size() - 1;
      }

      auto sort_descending = sort->getToken() != nullptr &&
          sort->getToken()->getType() == Token::T_DESC;

      /* create the sort spec */
      OrderBy::SortSpec sort_spec;
      sort_spec.column = col_index;
      sort_spec.descending = sort_descending;
      sort_specs.emplace_back(sort_spec);
    }
  }

  /* copy ast for child and swap out select lists, remove order by clause */
  auto child_ast = ast->deepCopy();
  child_ast->removeChildByIndex(0);
  child_ast->appendChild(child_sl, 0);
  child_ast->removeChildrenByType(ASTNode::T_ORDER_BY);

  return new OrderBy(
      ast->getChildren()[0]->getChildren().size(),
      sort_specs,
      buildQueryPlan(child_ast, repo));
}


void QueryPlanBuilder::extend(
    std::unique_ptr<QueryPlanBuilderInterface> other) {
  extensions_.emplace_back(std::move(other));
}

}
}
