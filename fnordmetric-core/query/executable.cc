/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "executable.h"

namespace fnordmetric {
namespace query {

Executable::Executable() : target_(nullptr) {}
Executable::~Executable() {}

SValue* Executable::expr(ASTNode* e) {
  printf("execute expression %p\n", e);

  e->debugPrint(2);
  switch (e->getType()) {

    case ASTNode::T_NEGATE_EXPR:
      return negExpr(e);

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

SValue* Executable::negExpr(ASTNode* e) {
  auto args = e->getChildren();
  assert(args.size() == 1);

  std::unique_ptr<SValue> val(expr(args[0]));

  if (val.get() == nullptr) {
    return nullptr;
  }

  SValue* result = nullptr;
  switch(val->getType()) {

    case SValue::T_INTEGER: {
      result = new SValue(val->getInteger() * -1);
      break;
    }

    case SValue::T_FLOAT: {
      result = new SValue(val->getFloat() * -1.0f);
      break;
    }

    case SValue::T_BOOL: {
      result = new SValue(!val->getBool());
      break;
    }

  }

  if (result == nullptr) {
    assert(0); // FIXPAUL
  } else {
    return result;
  }
}

SValue* Executable::addExpr(ASTNode* e) {
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

SValue* Executable::mulExpr(ASTNode* e) {
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


}
}
