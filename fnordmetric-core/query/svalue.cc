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

SValue::SValue() {
  memset(&data_, 0, sizeof(data_));
  data_.type = T_UNDEFINED;
}

SValue::SValue(int64_t integer_value) : SValue() {
  data_.type = T_INTEGER;
  data_.u.t_integer = integer_value;
}

SValue::SValue(double float_value) : SValue() {
  data_.type = T_FLOAT;
  data_.u.t_float = float_value;
}

SValue::SValue(bool bool_value) : SValue() {
  data_.type = T_BOOL;
  data_.u.t_bool = bool_value;
}

SValue::SValue(const SValue& copy) {
  memcpy(&data_, &copy.data_, sizeof(data_));
}

SValue::~SValue() {}

SValue::kSValueType SValue::getType() const {
  return data_.type;
}

int64_t SValue::getInteger() const {
  assert(data_.type == T_INTEGER);
  return data_.u.t_integer;
}

double SValue::getFloat() const {
  assert(data_.type == T_FLOAT);
  return data_.u.t_float;
}

bool SValue::getBool() const {
  assert(data_.type == T_BOOL);
  return data_.u.t_bool;
}

std::string SValue::makeUniqueKey(
    const std::vector<SValue*>& arr,
    size_t len) {
  size_t buf_len = sizeof(data_) * len;
  char* buf = static_cast<char*>(alloca(buf_len));

  for (int i = 0; i < len; ++i) {
    memcpy(buf + i * sizeof(data_), &arr[i]->data_, sizeof(data_));
  }

  return std::string(buf, buf_len);
}

std::vector<SValue*> SValue::loadFromKey(const std::string& key) {
  std::vector<SValue*> values;
  const char* buf = key.data();
  size_t len = key.size();

  assert(key.size() % sizeof(data_) == 0);

  for (size_t off = 0; off < len; off += sizeof(data_)) {
    auto svalue = new SValue();
    memcpy(&svalue->data_, &buf[off], sizeof(data_));
    values.push_back(svalue);
  }

  return values;
}

std::string SValue::toString() const {
  char buf[512];
  const char* str;
  size_t len;

  switch (data_.type) {

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

    case T_BOOL: {
      static const auto true_str = "true";
      static const auto false_str = "false";
      if (getBool()) {
        str = true_str;
        len = sizeof(true_str);
      } else {
        str = false_str;
        len = sizeof(false_str);
      }
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

    case Token::T_TRUE:
      return new SValue(true);

    case Token::T_FALSE:
      return new SValue(false);

    default:
      assert(0);
      return nullptr;

  }
}

}
}
