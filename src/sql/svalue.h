/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_SVALUE_H
#define _FNORDMETRIC_QUERY_SVALUE_H
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/types.h>

namespace fnordmetric {
namespace query {
class Token;

class SValue {
public:
  enum kSValueType {
    T_STRING,
    T_FLOAT,
    T_INTEGER,
    T_BOOL,
    T_TIMESTAMP,
    T_UNDEFINED
  };

  struct TypeError : public fnordmetric::util::RuntimeException {
    template <typename... T>
    TypeError(
        const char* message, T... args) :
        RuntimeException(message, args...) {}
  };

  static const char* getTypeName(kSValueType type);
  const char* getTypeName() const;

  explicit SValue();
  explicit SValue(const fnordmetric::StringType& string_value);
  explicit SValue(fnordmetric::IntegerType integer_value);
  explicit SValue(fnordmetric::FloatType float_value);
  explicit SValue(fnordmetric::BoolType bool_value);
  explicit SValue(fnordmetric::TimeType time_value);
  explicit SValue(const char* str_value, size_t len, bool copy);

  SValue(const SValue& copy);
  SValue& operator=(const SValue& copy);
  bool operator==(const SValue& other) const;
  ~SValue();

  static std::string makeUniqueKey(SValue* arr, size_t len);

  template <typename T> T getValue() const;
  template <typename T> bool testType() const;
  kSValueType getType() const;
  kSValueType testTypeWithNumericConversion() const;
  fnordmetric::IntegerType getInteger() const;
  fnordmetric::FloatType getFloat() const;
  fnordmetric::BoolType getBool() const;
  fnordmetric::TimeType getTimestamp() const;
  fnordmetric::StringType getString() const;
  std::string toString() const;
  bool tryNumericConversion();
  bool tryTimeConversion();

  static SValue* fromToken(const Token* token);

protected:
  struct {
    kSValueType type;
    union {
      fnordmetric::IntegerType t_integer;
      fnordmetric::FloatType t_float;
      fnordmetric::BoolType t_bool;
      uint64_t t_timestamp;
      struct {
        char* ptr;
        uint32_t len;
      } t_string;
    } u;
  } data_;
};

}
}
#endif
