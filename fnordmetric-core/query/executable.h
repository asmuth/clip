/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_EXECUTABLE_H
#define _FNORDMETRIC_QUERY_EXECUTABLE_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"
#include "svalue.h"

namespace fnordmetric {
namespace query {

class RowSink {
public:
  virtual void addRow(std::vector<std::unique_ptr<SValue>>&& row) = 0;
};

class Executable : public RowSink {
public:
  Executable();
  virtual ~Executable();
  virtual void execute() = 0;

  virtual size_t getNumCols() const = 0;
  virtual const std::vector<std::string>& getColumns() const = 0;

  void setTarget(RowSink* target) {
    target_ = target;
  }

protected:

  void emitRow(std::vector<std::unique_ptr<SValue>>&& row) {
    assert(target_ != nullptr);
    target_->addRow(std::move(row));
  }

  SValue* expr(ASTNode* e) {
    printf("execute expression %p\n", e);

    e->debugPrint(2);
    switch (e->getType()) {
      case ASTNode::T_ADD_EXPR:
        return addExpr(e);

      case ASTNode::T_MUL_EXPR:
        return mulExpr(e);

      case ASTNode::T_LITERAL:
        assert(e->getToken() != nullptr);
        return SValue::fromToken(e->getToken());

      default:
        assert(0); // FIXPAUL
    };
  }

  SValue* addExpr(ASTNode* e) {
    auto args = e->getChildren();
    assert(args.size() == 2);

    std::unique_ptr<SValue> lhs(expr(args[0]));
    std::unique_ptr<SValue> rhs(expr(args[1]));

    if (lhs.get() == nullptr || rhs.get() == nullptr) {
      return nullptr;
    }

    SValue* result = nullptr;
    switch(lhs->getType()) {

      case SValue::T_INTEGER: {
        switch(rhs->getType()) {
          case SValue::T_INTEGER: {
            result = new SValue(lhs->getInteger() + rhs->getInteger());
          }
        }
      }
    }

    if (result == nullptr) {
      assert(0); // FIXPAUL
    } else {
      return result;
    }
  }

  SValue* mulExpr(ASTNode* e) {
    auto args = e->getChildren();
    assert(args.size() == 2);

    std::unique_ptr<SValue> lhs(expr(args[0]));
    std::unique_ptr<SValue> rhs(expr(args[1]));

    if (lhs.get() == nullptr || rhs.get() == nullptr) {
      return nullptr;
    }

    SValue* result = nullptr;
    switch(lhs->getType()) {

      case SValue::T_INTEGER: {
        switch(rhs->getType()) {
          case SValue::T_INTEGER: {
            result = new SValue(lhs->getInteger() * rhs->getInteger());
          }
        }
      }
    }

    if (result == nullptr) {
      assert(0); // FIXPAUL
    } else {
      return result;
    }
  }

  RowSink* target_;
};

}
}
#endif
