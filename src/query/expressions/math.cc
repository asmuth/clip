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
#include "../symboltable.h"

namespace fnordmetric {
namespace query {

void addExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((int64_t) (lhs->getInteger() + rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) (lhs->getInteger() + rhs->getFloat()));
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((double) (lhs->getFloat() + rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) (lhs->getFloat() + rhs->getFloat()));
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

static SymbolTableEntry __add_symbol("add", &addExpr);

void subExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((int64_t) (lhs->getInteger() - rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) (lhs->getInteger() - rhs->getFloat()));
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((double) (lhs->getFloat() - rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) (lhs->getFloat() - rhs->getFloat()));
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

static SymbolTableEntry __sub_symbol("sub", &subExpr);

void mulExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((int64_t) (lhs->getInteger() * rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) (lhs->getInteger() * rhs->getFloat()));
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((double) (lhs->getFloat() * rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) (lhs->getFloat() * rhs->getFloat()));
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

static SymbolTableEntry __mul_symbol("mul", &mulExpr);

void divExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((int64_t) (lhs->getInteger() / rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) (lhs->getInteger() / rhs->getFloat()));
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((double) (lhs->getFloat() / rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) (lhs->getFloat() / rhs->getFloat()));
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

static SymbolTableEntry __div_symbol("div", &divExpr);

void modExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((int64_t) (lhs->getInteger() % rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue(fmod(lhs->getInteger(), rhs->getFloat()));
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue(fmod(lhs->getFloat(), rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue(fmod(lhs->getFloat(), rhs->getFloat()));
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

static SymbolTableEntry __mod_symbol("mod", &modExpr);

void powExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_INTEGER:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((int64_t) pow(lhs->getInteger(), rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) pow(lhs->getInteger(), rhs->getFloat()));
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->getType()) {
        case SValue::T_INTEGER:
          *out = SValue((double) pow(lhs->getFloat(), rhs->getInteger()));
          return;
        case SValue::T_FLOAT:
          *out = SValue((double) pow(lhs->getFloat(), rhs->getFloat()));
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

static SymbolTableEntry __pow_symbol("pow", &powExpr);

}
}
