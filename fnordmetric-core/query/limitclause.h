/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_LIMITCLAUSE_H
#define _FNORDMETRIC_QUERY_LIMITCLAUSE_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"
#include "executable.h"

namespace fnordmetric {
namespace query {

class LimitClause : public Executable {
public:

  static LimitClause* build(ASTNode* ast, TableRepository* repo) {
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

      return new LimitClause(limit, offset, planQuery(new_ast, repo));
    }

    return nullptr;
  }

  LimitClause(int limit, int offset, Executable* child) :
      limit_(limit),
      offset_(offset),
      child_(child),
      counter_(0) {
    child->setTarget(this);
  }

  void execute() override {
    child_->execute();
  }

  size_t getNumCols() const override {
    return child_->getNumCols();
  }

  bool nextRow(SValue* row, int row_len) override {
    if (counter_++ < offset_) {
      return true;
    }

    if (counter_ > (offset_ + limit_)) {
      return false;
    }

    emitRow(row, row_len);
    return true;
  }

  const std::vector<std::string>& getColumns() const override {
    return child_->getColumns();
  }

protected:
  size_t limit_;
  size_t offset_;
  Executable* child_;
  size_t counter_;
};

}
}
#endif
