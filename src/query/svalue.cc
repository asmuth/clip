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
#include "token.h"

namespace fnordmetric {
namespace query {

SValue::SValue() {
  memset(&data_, 0, sizeof(data_));
  data_.type = T_UNDEFINED;
}

SValue::~SValue() {
  // FIXPAUL free string!
}

SValue::SValue(const std::string& string_value) {
  data_.type = T_STRING;
  data_.u.t_string.len = string_value.size();
  data_.u.t_string.ptr = static_cast<char *>(malloc(data_.u.t_string.len));

  if (data_.u.t_string.ptr == nullptr) {
    RAISE(util::RuntimeException, "could not allocate SValue");
  }

  memcpy(
      data_.u.t_string.ptr,
      string_value.c_str(),
      data_.u.t_string.len);
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
  switch (copy.data_.type) {

    case T_STRING:
      data_.type = T_STRING;
      data_.u.t_string.len = copy.data_.u.t_string.len;
      data_.u.t_string.ptr = static_cast<char *>(malloc(data_.u.t_string.len));

      if (data_.u.t_string.ptr == nullptr) {
        RAISE(util::RuntimeException, "could not allocate SValue");
      }

      memcpy(
          data_.u.t_string.ptr,
          copy.data_.u.t_string.ptr,
          data_.u.t_string.len);

    default:
      memcpy(&data_, &copy.data_, sizeof(data_));
      break;

  }

}

SValue& SValue::operator=(const SValue& copy) {
  // FIXPAUL free old string!

  switch (copy.data_.type) {

    case T_STRING:
      data_.type = T_STRING;
      data_.u.t_string.len = copy.data_.u.t_string.len;
      data_.u.t_string.ptr = static_cast<char *>(malloc(data_.u.t_string.len));

      if (data_.u.t_string.ptr == nullptr) {
        RAISE(util::RuntimeException, "could not allocate SValue");
      }

      memcpy(
          data_.u.t_string.ptr,
          copy.data_.u.t_string.ptr,
          data_.u.t_string.len);

    default:
      memcpy(&data_, &copy.data_, sizeof(data_));
      break;

  }

  return *this;
}

bool SValue::operator==(const SValue& other) const {
  if (data_.type != other.data_.type) {
    return false;
  }

  switch (data_.type) {
    case T_STRING:
      return memcmp(
          data_.u.t_string.ptr,
          other.data_.u.t_string.ptr,
          data_.u.t_string.len) == 0;

    default:
      return memcmp(&data_, &other.data_, sizeof(data_)) == 0;

  }
}

SValue::kSValueType SValue::getType() const {
  return data_.type;
}

int64_t SValue::getInteger() const {
  assert(data_.type == T_INTEGER);
  return data_.u.t_integer;
}

double SValue::getFloat() const {
  switch (data_.type) {
    case T_INTEGER:
      return data_.u.t_integer;
    case T_FLOAT:
      return data_.u.t_float;

    default:
      RAISE(
          TypeError,
          "can't convert %s '%s' to Float",
          SValue::getTypeName(data_.type),
          toString().c_str());

  }
  if (data_.type == T_INTEGER) {
  }

  if (data_.type == T_INTEGER) {
  }

  return data_.u.t_float;
}

bool SValue::getBool() const {
  assert(data_.type == T_BOOL);
  return data_.u.t_bool;
}

const std::string SValue::getString() const {
  if (data_.type == T_STRING) {
    return std::string(data_.u.t_string.ptr, data_.u.t_string.len);
  } else {
    toString();
  }
}

std::string SValue::makeUniqueKey(SValue* arr, size_t len) {
  size_t buf_len = sizeof(data_) * len;
  char* buf = static_cast<char*>(alloca(buf_len));

  for (int i = 0; i < len; ++i) {
    memcpy(buf + i * sizeof(data_), &arr[i].data_, sizeof(data_));
  }

  return std::string(buf, buf_len);
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

    case T_STRING: {
      return getString();
    }

    case T_UNDEFINED: {
      static const auto undef_str = "undefined";
      str = undef_str;
      len = sizeof(undef_str);
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

    case Token::T_STRING:
      return new SValue(token->getString());

    default:
      assert(0);
      return nullptr;

  }
}


const char* SValue::getTypeName(kSValueType type) {
  switch (type) {
    case T_STRING:
      return "String";
    case T_FLOAT:
      return "Float";
    case T_INTEGER:
      return "Integer";
    case T_BOOL:
      return "Boolean";
    case T_UNDEFINED:
      return "Undefined";
  }
}

template<> double SValue::getValue<double>() const {
  return getFloat();
}

template<> std::string SValue::getValue<std::string>() const {
  return toString();
}

template<> bool SValue::testType<double>() const {
  return true; // FIXPAUL
}

template<> bool SValue::testType<std::string>() const {
  return true;
}

}
}
