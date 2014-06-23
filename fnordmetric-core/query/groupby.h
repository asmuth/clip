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

namespace fnordmetric {
namespace query {

class GroupBy : public Executable {
public:

  struct Group {
    std::vector<SValue*> row;
  };

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
      std::vector<ASTNode*> group_exprs;
      for (const auto& group_expr : child->getChildren()) {
        auto e = group_expr->deepCopy();
        rewriteAST(e, child_sl);
        group_exprs.emplace_back(e);
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

      /* resolve aggregation functions */
      child_ast->debugPrint(2);
      select_list->debugPrint(2);

      return new GroupBy(
          select_list,
          group_exprs,
          planQuery(child_ast, repo));
    }

    return nullptr;
  }

  GroupBy(
      ASTNode* select_list,
      std::vector<ASTNode*> group_exprs,
      Executable* child) :
      select_list_(select_list),
      group_exprs_(group_exprs),
      child_(child) {
    child->setTarget(this);

    for (const auto& col : select_list_->getChildren()) {
      output_expressions_.push_back(col->getChildren()[0]);
      columns_.push_back("unnamed");
    }
  }

  void execute() override {
    child_->execute();

    for (auto& pair : groups_) {
      auto& group = pair.second;
      setCurrentRow(&group.row);

      std::vector<SValue*> row;
      for (const auto& e : output_expressions_) {
        row.emplace_back(expr(e));
      }

      emitRow(row);
    }
  }

  bool nextRow(std::vector<SValue*> row) override {
    setCurrentRow(&row);

    /* execute group expressions */
    std::vector<SValue*> key;
    for (const auto& e : group_exprs_) {
      key.push_back(expr(e));
    }

    /* stringify expression results into group key */
    SValue** row_ptr = key.data();
    auto key_str = SValue::makeUniqueKey(key, key.size());

    auto group = groups_.find(key_str);
    if (group == groups_.end()) {
      groups_[key_str].row = row;
    } else {
      // discard value...
    }

    return true;
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

protected:

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
      node->setResolvedSymbol(col_index);
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


  std::vector<ASTNode*> output_expressions_;
  std::vector<std::string> columns_;
  std::unordered_map<std::string, Group> groups_;
  ASTNode* select_list_;
  std::vector<ASTNode*> group_exprs_;
  Executable* child_;
};

}
}
#endif
