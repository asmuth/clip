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

  typedef std::string StringType;
  typedef double FloatType;
  typedef int64_t IntegerType;
  typedef bool BoolType;
  typedef time_t TimeType;

  struct TypeError : public fnordmetric::util::RuntimeException {
    template <typename... T>
    TypeError(
        const char* message, T... args) :
        RuntimeException(message, args...) {}
  };

  static const char* getTypeName(kSValueType type);
  const char* getTypeName() const;

  explicit SValue();
  explicit SValue(const StringType& string_value);
  explicit SValue(IntegerType integer_value);
  explicit SValue(FloatType float_value);
  explicit SValue(BoolType bool_value);
  explicit SValue(TimeType time_value);
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
  IntegerType getInteger() const;
  FloatType getFloat() const;
  BoolType getBool() const;
  TimeType getTimestamp() const;
  StringType getString() const;
  std::string toString() const;
  bool tryNumericConversion();
  bool tryTimeConversion();

  static SValue* fromToken(const Token* token);

protected:
  struct {
    kSValueType type;
    union {
      IntegerType t_integer;
      FloatType t_float;
      BoolType t_bool;
      TimeType t_timestamp;
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
