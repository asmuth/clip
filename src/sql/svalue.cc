/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <stdint.h>
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/sql/svalue.h>
#include <fnordmetric/util/format.h>

namespace fnordmetric {
namespace query {

SValue::SValue() {
  memset(&data_, 0, sizeof(data_));
  data_.type = T_UNDEFINED;
}

SValue::~SValue() {
  // FIXPAUL free string!
}

SValue::SValue(const fnordmetric::StringType& string_value) {
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

SValue::SValue(fnordmetric::IntegerType integer_value) : SValue() {
  data_.type = T_INTEGER;
  data_.u.t_integer = integer_value;
}

SValue::SValue(fnordmetric::FloatType float_value) : SValue() {
  data_.type = T_FLOAT;
  data_.u.t_float = float_value;
}

SValue::SValue(fnordmetric::BoolType bool_value) : SValue() {
  data_.type = T_BOOL;
  data_.u.t_bool = bool_value;
}

SValue::SValue(fnordmetric::TimeType time_value) : SValue() {
  data_.type = T_TIMESTAMP;
  data_.u.t_timestamp = static_cast<uint64_t>(time_value);
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

fnordmetric::IntegerType SValue::getInteger() const {
  switch (data_.type) {

    case T_INTEGER:
      return data_.u.t_integer;

    case T_STRING:
      try {
        return std::stoi(getString());
      } catch (std::exception e) {
        /* fallthrough */
      }

    default:
      RAISE(
          TypeError,
          "can't convert %s '%s' to Float",
          SValue::getTypeName(data_.type),
          toString().c_str());

  }

  return 0;
}

fnordmetric::FloatType SValue::getFloat() const {
  switch (data_.type) {

    case T_INTEGER:
      return data_.u.t_integer;

    case T_FLOAT:
      return data_.u.t_float;

    case T_STRING:
      try {
        return std::stod(getString());
      } catch (std::exception e) {
        /* fallthrough */
      }

    default:
      RAISE(
          TypeError,
          "can't convert %s '%s' to Float",
          SValue::getTypeName(data_.type),
          toString().c_str());

  }

  return 0;
}

fnordmetric::BoolType SValue::getBool() const {
  if (data_.type != T_BOOL) {
    RAISE(
       TypeError,
        "can't convert %s '%s' to Bool",
        SValue::getTypeName(data_.type),
        toString().c_str());
  }

  return data_.u.t_bool;
}

fnordmetric::TimeType SValue::getTimestamp() const {
  switch (getType()) {

    case T_TIMESTAMP:
      return data_.u.t_timestamp;

    default:
      RAISE(
         TypeError,
          "can't convert %s '%s' to Timestamp",
          SValue::getTypeName(data_.type),
          toString().c_str());
      time_t ts;
      time(&ts);

  }
}

fnordmetric::StringType SValue::getString() const {
  if (data_.type == T_STRING) {
    return std::string(data_.u.t_string.ptr, data_.u.t_string.len);
  } else {
    return toString();
  }
}

std::string SValue::makeUniqueKey(SValue* arr, size_t len) {
  std::string key;

  for (int i = 0; i < len; ++i) {
    key.append(arr[i].toString());
    key.append("\x00");
  }

  return key;
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

    case T_TIMESTAMP: {
      return util::format::formatTime(getTimestamp());
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

    case Token::T_TRUE:
      return new SValue(true);

    case Token::T_FALSE:
      return new SValue(false);

    case Token::T_NUMERIC: {
      auto sval = new SValue(token->getString());
      sval->tryNumericConversion();
      return sval;
    }

    case Token::T_STRING:
      return new SValue(token->getString());

    default:
      RAISE(util::RuntimeException, "can't cast Token to SValue");
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
    case T_TIMESTAMP:
      return "Timestamp";
    case T_UNDEFINED:
      return "Undefined";
  }
}

const char* SValue::getTypeName() const {
  return SValue::getTypeName(data_.type);
}

template <> fnordmetric::BoolType SValue::getValue<fnordmetric::BoolType>() const {
  return getBool();
}

template <> fnordmetric::IntegerType SValue::getValue<fnordmetric::IntegerType>() const {
  return getInteger();
}

template <> fnordmetric::FloatType SValue::getValue<fnordmetric::FloatType>() const {
  return getFloat();
}

template <> fnordmetric::StringType SValue::getValue<fnordmetric::StringType>() const {
  return toString();
}

template <> fnordmetric::TimeType SValue::getValue<fnordmetric::TimeType>() const {
  return getTimestamp();
}

// FIXPAUL: smarter type detection
template <> bool SValue::testType<fnordmetric::BoolType>() const {
  return data_.type == T_BOOL;
}

template <> bool SValue::testType<fnordmetric::TimeType>() const {
  return data_.type == T_TIMESTAMP;
}

template <> bool SValue::testType<fnordmetric::IntegerType>() const {
  if (data_.type == T_INTEGER) {
    return true;
  }

  auto str = toString();
  const char* c = str.c_str();

  if (*c == '-') {
    ++c;
  }

  for (; *c != 0; ++c) {
    if (*c < '0' || *c > '9') {
      return false;
    }
  }

  return true;
}

template <> bool SValue::testType<fnordmetric::FloatType>() const {
  if (data_.type == T_FLOAT) {
    return true;
  }

  auto str = toString();
  bool dot = false;
  const char* c = str.c_str();

  if (*c == '-') {
    ++c;
  }

  for (; *c != 0; ++c) {
    if (*c >= '0' && *c <= '9') {
      continue;
    }

    if (*c == '.' || *c == ',') {
      if (dot) {
        return false;
      } else {
        dot = true;
      }
      continue;
    }

    return false;
  }

  return true;
}

template <> bool SValue::testType<std::string>() const {
  return true;
}

SValue::kSValueType SValue::testTypeWithNumericConversion() const {
  if (testType<fnordmetric::IntegerType>()) return T_INTEGER;
  if (testType<fnordmetric::FloatType>()) return T_FLOAT;
  return getType();
}

bool SValue::tryNumericConversion() {
  if (testType<fnordmetric::IntegerType>()) {
    fnordmetric::IntegerType val = getValue<fnordmetric::IntegerType>();
    data_.type = T_INTEGER;
    data_.u.t_integer = val;
    return true;
  }

  if (testType<fnordmetric::FloatType>()) {
    fnordmetric::FloatType val = getValue<fnordmetric::FloatType>();
    data_.type = T_FLOAT;
    data_.u.t_float = val;
    return true;
  }

  return false;
}

bool SValue::tryTimeConversion() {
  if (!tryNumericConversion()) {
    return false;
  }

  time_t ts = getInteger();
  data_.type = T_TIMESTAMP;
  // FIXPAUL take a smart guess if this is milli, micro, etc
  data_.u.t_timestamp = ts * 1000000;
  return true;
}


}
}
