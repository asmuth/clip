/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <assert.h>
#include "svalue.h"

namespace fnordmetric {
namespace query {

SValue::SValue(int64_t integer_value) : type_(T_INTEGER) {
  data_.t_integer = integer_value;
}

SValue::SValue(double float_value) : type_(T_FLOAT) {
  data_.t_float = float_value;
}

SValue::~SValue() {}

SValue::kSValueType SValue::getType() const {
  return type_;
}

int64_t SValue::getInteger() const {
  assert(type_ == T_INTEGER);
  return data_.t_integer;
}

double SValue::getFloat() const {
  assert(type_ == T_FLOAT);
  return data_.t_float;
}

std::string SValue::toString() const {
  char buf[512];
  const char* str;
  size_t len;

  switch (type_) {

    case T_INTEGER: {
      len = snprintf(buf, sizeof(buf), "%" PRId64, getInteger());
      str = buf;
      break;
    }

    case T_FLOAT: {
      len = snprintf(buf, sizeof(buf), "%f", getFloat());
      str = buf;
      break;
    }

  }

  return std::string(str, len);
}

SValue* SValue::fromToken(const Token* token) {
  switch (token->getType()) {

    case Token::T_NUMERIC: {
      if (token->isDouble()) {
        return new SValue(token->getDouble());
      } else {
        return new SValue(token->getInteger());
      }
    }

    default:
      return nullptr;

  }
}

}
}
