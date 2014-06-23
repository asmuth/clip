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

  static GroupBy* build(ASTNode* ast, TableRepository* repo) {
    if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 3) {
      return nullptr;
    }

    for (const auto& child : ast->getChildren()) {
      int limit = 0;
      int offset = 0;

      if (child->getType() != ASTNode::T_GROUP_BY) {
        continue;
      }

      auto group_exprs = child->getChildren();
      assert(group_exprs.size() > 0);
      std::unique_ptr<Grouper> grouper(new Grouper(group_exprs.size()));


      auto child_ast = ast->deppCopy();
      // FIXPAUL: stash select list, group by list
      // FIXPAUL: rewrite select list for child (remove aggr funcs)
      // FIXPAUL: resolve own select list, group by list

      assert(0);
    }

    return nullptr;
  }

  GroupBy(Executable* child) :
      child_(child) {
    child->setTarget(this);
  }

  void execute() override {
    child_->execute();
  }

  size_t getNumCols() const override {
    return child_->getNumCols();
  }

  bool nextRow(std::vector<SValue*> row) override {
    return true;
  }

  const std::vector<std::string>& getColumns() const override {
    return child_->getColumns();
  }

protected:
  Executable* child_;
};

}
}
#endif
