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
#include <assert.h>
#include <math.h>
#include <string.h>
#include <fnordmetric/sql/runtime/symboltable.h>
#include <fnordmetric/sql/svalue.h>

namespace fnordmetric {
namespace query {

void addExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 2) {
    RAISE(
        util::RuntimeException,
        "wrong number of arguments for add. expected: 2, got: %i", argc);
  }

  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
      switch(rhs->testTypeWithNumericConversion()) {
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
      switch(rhs->testTypeWithNumericConversion()) {
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

  *out = SValue(lhs->toString() + rhs->toString());
}

SymbolTableEntry __add_symbol("add", &addExpr);

void subExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
      switch(rhs->testTypeWithNumericConversion()) {
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
      switch(rhs->testTypeWithNumericConversion()) {
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

  RAISE(util::RuntimeException, "can't subtract %s and %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

static SymbolTableEntry __sub_symbol("sub", &subExpr);

void mulExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
      switch(rhs->testTypeWithNumericConversion()) {
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
      switch(rhs->testTypeWithNumericConversion()) {
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

  RAISE(util::RuntimeException, "can't multiply %s and %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

static SymbolTableEntry __mul_symbol("mul", &mulExpr);

void divExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
      switch(rhs->testTypeWithNumericConversion()) {
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
      switch(rhs->testTypeWithNumericConversion()) {
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

  RAISE(util::RuntimeException, "can't divide %s and %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

static SymbolTableEntry __div_symbol("div", &divExpr);

void modExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
      switch(rhs->testTypeWithNumericConversion()) {
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
      switch(rhs->testTypeWithNumericConversion()) {
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

  RAISE(util::RuntimeException, "can't modulo %s and %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

static SymbolTableEntry __mod_symbol("mod", &modExpr);

void powExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  assert(argc == 2);
  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
      switch(rhs->testTypeWithNumericConversion()) {
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
      switch(rhs->testTypeWithNumericConversion()) {
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

  RAISE(util::RuntimeException, "can't pow %s and %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

static SymbolTableEntry __pow_symbol("pow", &powExpr);

}
}
