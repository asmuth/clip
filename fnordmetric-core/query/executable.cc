/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "executable.h"

namespace fnordmetric {
namespace query {

Executable::Executable() : target_(nullptr) {}
Executable::~Executable() {}

SValue* Executable::expr(ASTNode* e) {
  switch (e->getType()) {

    case ASTNode::T_NEGATE_EXPR:
      return negExpr(e);

    case ASTNode::T_ADD_EXPR:
      return addExpr(e);

    case ASTNode::T_SUB_EXPR:
      return subExpr(e);

    case ASTNode::T_MUL_EXPR:
      return mulExpr(e);

    case ASTNode::T_DIV_EXPR:
      return divExpr(e);

    case ASTNode::T_MOD_EXPR:
      return modExpr(e);

    case ASTNode::T_POW_EXPR:
      return powExpr(e);

    case ASTNode::T_LITERAL:
      assert(e->getToken() != nullptr);
      return SValue::fromToken(e->getToken());

    case ASTNode::T_RESOLVED_COLUMN:
      assert(e->getResolvedSymbol() >= 0);
      assert(cur_row_->size() > e->getResolvedSymbol());
      return (*cur_row_)[e->getResolvedSymbol()];

    default:
      printf("error: cant execute expression\n");
      e->debugPrint();
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

  switch(val->getType()) {
    case SValue::T_INTEGER:
      return new SValue(val->getInteger() * -1);
    case SValue::T_FLOAT:
      return new SValue(val->getFloat() * -1.0f);
    case SValue::T_BOOL:
      return new SValue(!val->getBool());
    default:
      break;
  }

  assert(0); // FIXPAUL
}

#define ARITHMETIC_EXPR_IMPL(name, op)                                        \
  SValue* Executable::name(ASTNode* e) {                                      \
    auto args = e->getChildren();                                             \
    assert(args.size() == 2);                                                 \
    std::unique_ptr<SValue> lhs(expr(args[0]));                               \
    std::unique_ptr<SValue> rhs(expr(args[1]));                               \
                                                                              \
    if (lhs.get() == nullptr || rhs.get() == nullptr) {                       \
      return nullptr;                                                         \
    }                                                                         \
                                                                              \
    switch(lhs->getType()) {                                                  \
      case SValue::T_INTEGER:                                                 \
        switch(rhs->getType()) {                                              \
          case SValue::T_INTEGER:                                             \
            return new SValue((int64_t) (                                     \
                lhs->getInteger() op rhs->getInteger()));                     \
          case SValue::T_FLOAT:                                               \
            return new SValue((double) (                                      \
                lhs->getInteger() op rhs->getFloat()));                       \
          default:                                                            \
            break;                                                            \
        }                                                                     \
        break;                                                                \
      case SValue::T_FLOAT:                                                   \
        switch(rhs->getType()) {                                              \
          case SValue::T_INTEGER:                                             \
            return new SValue((double) (                                      \
                lhs->getFloat() op rhs->getInteger()));                       \
          case SValue::T_FLOAT:                                               \
            return new SValue((double) (                                      \
                lhs->getFloat() op rhs->getFloat()));                         \
          default:                                                            \
            break;                                                            \
        }                                                                     \
        break;                                                                \
      default:                                                                \
        break;                                                               \
    }                                                                         \
                                                                              \
    assert(0);                                                                \
  }

ARITHMETIC_EXPR_IMPL(addExpr, +)
ARITHMETIC_EXPR_IMPL(subExpr, -)
ARITHMETIC_EXPR_IMPL(mulExpr, *)
ARITHMETIC_EXPR_IMPL(divExpr, /)

SValue* Executable::modExpr(ASTNode* e) {
  auto args = e->getChildren();
  assert(args.size() == 2);

  std::unique_ptr<SValue> lhs(expr(args[0]));
  std::unique_ptr<SValue> rhs(expr(args[1]));

  if (lhs.get() == nullptr || rhs.get() == nullptr) {
    return nullptr;
  }

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          return new SValue((int64_t) (lhs->getInteger() % rhs->getInteger()));
        case SValue::T_FLOAT:
          return new SValue(fmod(lhs->getInteger(), rhs->getFloat()));
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          return new SValue(fmod(lhs->getFloat(), rhs->getInteger()));
        case SValue::T_FLOAT:
          return new SValue(fmod(lhs->getFloat(), rhs->getFloat()));
        default:
          break;
      }
      break;
    default:
      break;
  }

  assert(0); // FIXPAUL
}

SValue* Executable::powExpr(ASTNode* e) {
  auto args = e->getChildren();
  assert(args.size() == 2);

  std::unique_ptr<SValue> lhs(expr(args[0]));
  std::unique_ptr<SValue> rhs(expr(args[1]));

  if (lhs.get() == nullptr || rhs.get() == nullptr) {
    return nullptr;
  }

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          return new SValue((int64_t) pow(lhs->getInteger(), rhs->getInteger()));
        case SValue::T_FLOAT:
          return new SValue((double) pow(lhs->getInteger(), rhs->getFloat()));
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          return new SValue((double) pow(lhs->getFloat(), rhs->getInteger()));
        case SValue::T_FLOAT:
          return new SValue((double) pow(lhs->getFloat(), rhs->getFloat()));
        default:
          break;
      }
      break;
    default:
      break;
  }

  assert(0); // FIXPAUL
}

}
}
