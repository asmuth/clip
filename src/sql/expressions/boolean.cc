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
#include <fnordmetric/sql/expressions/boolean.h>
#include <fnord-base/exception.h>

namespace fnordmetric {
namespace query {
namespace expressions {

void eqExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 2) {
    RAISE(
        kRuntimeError,
        "wrong number of arguments for eq. expected: 2, got: %i", argc);
  }

  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
    case SValue::T_TIMESTAMP:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getInteger() == rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getFloat() == rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_FLOAT:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getFloat() == rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    default:
      break;
  }

  if (lhs->getType() == SValue::T_STRING ||
      rhs->getType() == SValue::T_STRING) {
    *out = SValue(lhs->toString() == rhs->toString());
    return;
  }

  RAISE(kRuntimeError, "can't compare %s with %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

void neqExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  SValue ret;
  eqExpr(scratchpad, argc, argv, &ret);
  *out = SValue(!ret.getValue<bool>());
}

void andExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 2) {
    RAISE(
        kRuntimeError,
        "wrong number of arguments for and. expected: 2, got: %i", argc);
  }

  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_BOOL:
      switch(rhs->getType()) {
        case SValue::T_BOOL:
          *out = SValue(lhs->getBool() && rhs->getBool());
          return;
        default:
          break;
      }
    default:
      break;
  }

  RAISE(kRuntimeError, "can't AND %s with %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

void orExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 2) {
    RAISE(
        kRuntimeError,
        "wrong number of arguments for or. expected: 2, got: %i", argc);
  }

  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->getType()) {
    case SValue::T_BOOL:
      switch(rhs->getType()) {
        case SValue::T_BOOL:
          *out = SValue(lhs->getBool() || rhs->getBool());
          return;
        default:
          break;
      }
    default:
      break;
  }

  RAISE(kRuntimeError, "can't OR %s with %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

void negExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 1) {
    RAISE(
        kRuntimeError,
        "wrong number of arguments for neg. expected: 1, got: %i", argc);
  }

  SValue* val = argv;

  switch(val->testTypeWithNumericConversion()) {
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

  RAISE(kRuntimeError, "can't negate %s",
      val->getTypeName());
}

void ltExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 2) {
    RAISE(
        kRuntimeError,
        "wrong number of arguments for ltExpr. expected: 2, got: %i", argc);
  }

  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
    case SValue::T_TIMESTAMP:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getInteger() < rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getFloat() < rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_FLOAT:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getFloat() < rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    default:
      break;
  }

  if (lhs->getType() == SValue::T_STRING ||
      rhs->getType() == SValue::T_STRING) {
    *out = SValue(lhs->toString() < rhs->toString());
    return;
  }

  RAISE(kRuntimeError, "can't compare %s with %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

void lteExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 2) {
    RAISE(
        kRuntimeError,
        "wrong number of arguments for lteExpr. expected: 2, got: %i", argc);
  }

  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
    case SValue::T_TIMESTAMP:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getInteger() <= rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getFloat() <= rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_FLOAT:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getFloat() <= rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    default:
      break;
  }

  if (lhs->getType() == SValue::T_STRING ||
      rhs->getType() == SValue::T_STRING) {
    *out = SValue(lhs->toString() <= rhs->toString());
    return;
  }

  RAISE(kRuntimeError, "can't compare %s with %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

void gtExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 2) {
    RAISE(
        kRuntimeError,
        "wrong number of arguments for gtExpr. expected: 2, got: %i", argc);
  }

  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
    case SValue::T_TIMESTAMP:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getInteger() > rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getFloat() > rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_FLOAT:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getFloat() > rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    default:
      break;
  }

  if (lhs->getType() == SValue::T_STRING ||
      rhs->getType() == SValue::T_STRING) {
    *out = SValue(lhs->toString() > rhs->toString());
    return;
  }

  RAISE(kRuntimeError, "can't compare %s with %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

void gteExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  if (argc != 2) {
    RAISE(
        kRuntimeError,
        "wrong number of arguments for gteExpr. expected: 2, got: %i", argc);
  }

  SValue* lhs = argv;
  SValue* rhs = argv + 1;

  switch(lhs->testTypeWithNumericConversion()) {
    case SValue::T_INTEGER:
    case SValue::T_TIMESTAMP:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getInteger() >= rhs->getInteger());
          return;
        case SValue::T_FLOAT:
          *out = SValue(lhs->getFloat() >= rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    case SValue::T_FLOAT:
      switch(rhs->testTypeWithNumericConversion()) {
        case SValue::T_INTEGER:
        case SValue::T_FLOAT:
        case SValue::T_TIMESTAMP:
          *out = SValue(lhs->getFloat() >= rhs->getFloat());
          return;
        default:
          break;
      }
      break;
    default:
      break;
  }

  if (lhs->getType() == SValue::T_STRING ||
      rhs->getType() == SValue::T_STRING) {
    *out = SValue(lhs->toString() >= rhs->toString());
    return;
  }

  RAISE(kRuntimeError, "can't compare %s with %s",
      lhs->getTypeName(),
      rhs->getTypeName());
}

}
}
}
