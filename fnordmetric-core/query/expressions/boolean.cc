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
#include "../svalue.h"

namespace fnordmetric {
namespace query {

void eqExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue(lhs->getInteger() == rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getInteger() == rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue(lhs->getFloat() == rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getFloat() == rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    default:
      break;
  }

  assert(0);
}

static SymbolTableEntry __eq_symbol("eq", &eqExpr);

void andExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_BOOL:
      switch(rhs->getType()) {
        case SValue::T_BOOL:
          *out = SValue(lhs->getBool() && rhs->getBool());
          return;
      }
  }

  assert(0);
}

static SymbolTableEntry __and_symbol("and", &andExpr);

void orExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_BOOL:
      switch(rhs->getType()) {
        case SValue::T_BOOL:
          *out = SValue(lhs->getBool() || rhs->getBool());
          return;
      }
  }

  assert(0);
}

static SymbolTableEntry __or_symbol("or", &orExpr);

void negExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 1);
  SValue* val = argv;

  switch(val->getType()) {
    case SValue::T_INTEGER:
      *out = SValue(val->getInteger() * -1);
      return;
    case SValue::T_FLOAT:
      *out = SValue(val->getFloat() * -1.0f);
      return;
    case SValue::T_BOOL:
      *out = SValue(!val->getBool());
      return;
    default:
      break;
  }

  assert(0);
}

static SymbolTableEntry __neg_symbol("neg", &negExpr);

void ltExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue(lhs->getInteger() < rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getInteger() < rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue(lhs->getFloat() < rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getFloat() < rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    default:
      break;
  }

  assert(0);
}

static SymbolTableEntry __lt_symbol("lt", &ltExpr);

void gtExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue(lhs->getInteger() > rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getInteger() > rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue(lhs->getFloat() > rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getFloat() > rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    default:
      break;
  }

  assert(0); // FIXPAUL
}

static SymbolTableEntry __gt_symbol("gt", &gtExpr);

}
}
