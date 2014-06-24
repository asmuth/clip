/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_GROUPBY_H
#define _FNORDMETRIC_QUERY_GROUPBY_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"
#include "executable.h"
#include "grouper.h"
#include "symboltable.h"
#include "compile.h"

namespace fnordmetric {
namespace query {

class GroupBy : public Executable {
public:

  static GroupBy* build(ASTNode* ast, TableRepository* repo) {
    if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 3) {
      return nullptr;
    }

    assert(ast->getChildren()[0]->getType() == ASTNode::T_SELECT_LIST);

    for (const auto& child : ast->getChildren()) {
      int limit = 0;
      int offset = 0;

      if (child->getType() != ASTNode::T_GROUP_BY) {
        continue;
      }

      /* FIXPAUL resolve aliases in group list from select list, return error
         if alias contains aggregate func */

      /* copy select list and group expressions and resolve fields*/
      auto child_sl = new ASTNode(ASTNode::T_SELECT_LIST);
      auto select_list = ast->getChildren()[0]->deepCopy();
      rewriteAST(select_list, child_sl);

      /* copy all group expressions and resolve fields */
      ASTNode group_exprs(ASTNode::T_GROUP_BY);
      for (const auto& group_expr : child->getChildren()) {
        auto e = group_expr->deepCopy();
        rewriteAST(e, child_sl);
        group_exprs.appendChild(e);
      }

      /* copy ast for child and swap out select lists*/
      auto child_ast = ast->deepCopy();
      child_ast->removeChild(0);
      child_ast->appendChild(child_sl, 0);

      /* remove group by clause from child ast */
      const auto& child_ast_children = child_ast->getChildren();
      for (int i = 0; i < child_ast_children.size(); ++i) {
        if (child_ast_children[i]->getType() == ASTNode::T_GROUP_BY) {
          child_ast->removeChild(i);
          break;
        }
      }

      /* compile select and group expressions */
      size_t select_scratchpad_len = 0;
      auto select_expr = compileAST(select_list, &select_scratchpad_len);
      printf("SCRATCHPAD LEN: %lu\n", select_scratchpad_len);

      size_t group_scratchpad_len = 0;
      auto group_expr = compileAST(&group_exprs, &group_scratchpad_len);
      assert(group_scratchpad_len == 0);
      //child_ast->debugPrint(2);
      //select_list->debugPrint(2);

      std::vector<std::string> column_names;
      for (const auto& col : select_list->getChildren()) {
        column_names.push_back("unnamed");
      }

      return new GroupBy(
          std::move(column_names),
          select_expr,
          group_expr,
          select_scratchpad_len,
          planQuery(child_ast, repo));
    }

    return nullptr;
  }

  GroupBy(
      std::vector<std::string>&& columns,
      CompiledExpression* select_expr,
      CompiledExpression* group_expr,
      size_t scratchpad_size,
      Executable* child) :
      columns_(std::move(columns)),
      select_expr_(select_expr),
      group_expr_(group_expr),
      scratchpad_size_(scratchpad_size),
      child_(child) {
    child->setTarget(this);
  }

  void execute() override {
    child_->execute();

    for (auto& pair : groups_) {
      auto& row = pair.second.row;
      emitRow(row.data(), row.size());
    }
  }

  bool nextRow(SValue* row, int row_len) override {
    SValue out[128]; // FIXPAUL
    int out_len;

    /* execute group expression */
    if (group_expr_ != nullptr) {
      executeExpression(group_expr_, nullptr, row_len, row, &out_len, out);
    }

    /* stringify expression results into group key */
    auto key_str = SValue::makeUniqueKey(out, out_len);

    /* get group */
    Group* group = nullptr;

    auto group_iter = groups_.find(key_str);
    if (group_iter == groups_.end()) {
      group = &groups_[key_str];
      group->scratchpad = malloc(scratchpad_size_);
      memset(group->scratchpad, 0, scratchpad_size_);
    } else {
      group = &group_iter->second;
    }

    /* execute select expresion and save results */
    executeExpression(
        select_expr_,
        group->scratchpad,
        row_len,
        row,
        &out_len,
        out);

    std::vector<SValue> row_vec;
    for (int i = 0; i < out_len; i++) {
      row_vec.push_back(out[i]);
    }

    /* update group */
    group->row = row_vec;

    return true;
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

protected:

  struct Group {
    std::vector<SValue> row;
    void* scratchpad;
  };

  static bool rewriteAST(ASTNode* node, ASTNode* target_select_list) {
    if (node->getType() == ASTNode::T_COLUMN_NAME) {
      auto col_index = -1;

      /* check if this columns is already in the select list */
      const auto& candidates = target_select_list->getChildren();
      for (int i = 0; i < candidates.size(); ++i) {
        //if (compareColumns(node, 
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
        if (!rewriteAST(child, target_select_list)) {
          return false;
        }
      }

      return true;
    }
  }

/*
  static bool resolveMethodCalls(
      ASTNode* node,
      bool allow_aggregate) {
    if (node->getType() == ASTNode::T_METHOD_CALL) {
      if (!resolveMethodCall(node, allow_aggregate)) {
        return false;
      }
    } else {
      for (const auto& child : node->getChildren()) {
        if (!resolveMethodCalls(child, allow_aggregate)) {
          return false;
        }
      }
    }

    return true;
  }

  static bool resolveMethodCall(
      ASTNode* node,
      bool allow_aggregate) {
    auto method_name = node->getToken();
    assert(method_name && *method_name == Token::T_IDENTIFIER);

    auto method = lookupSymbol(method_name->getString());

    if (!allow_aggregate && method->isAggregate()) {
      fprintf(stderr, "error: unexpected aggregate method %s\n",
          method_name->getString().c_str());
      return false;
    }

    if (method == nullptr) {
      fprintf(stderr, "error: cant resolve method %s\n",
          method_name->getString().c_str());
      return false;
    }

    return true;
  }

*/

  std::vector<std::string> columns_;
  CompiledExpression* select_expr_;
  CompiledExpression* group_expr_;
  size_t scratchpad_size_;
  Executable* child_;
  std::unordered_map<std::string, Group> groups_;
};

}
}
#endif
