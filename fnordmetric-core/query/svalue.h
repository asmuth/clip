/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_SVALUE_H
#define _FNORDMETRIC_QUERY_SVALUE_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"

namespace fnordmetric {
namespace query {

class SValue {
public:
  enum kSValueType {
    T_STRING,
    T_FLOAT,
    T_INTEGER,
    T_BOOL,
    T_UNDEFINED
  };

  explicit SValue();
  explicit SValue(int64_t integer_value);
  explicit SValue(double float_value);
  explicit SValue(bool bool_value);
  explicit SValue(const char* str_value, size_t len, bool copy);
  SValue(const SValue& copy);
  SValue& operator=(const SValue& copy) = delete;
  ~SValue();

  static std::string makeUniqueKey(const std::vector<SValue*>& arr, size_t len);
  static std::vector<SValue*> loadFromKey(const std::string& key);

  kSValueType getType() const;
  int64_t getInteger() const;
  double getFloat() const;
  bool getBool() const;
  std::string toString() const;

  static SValue* fromToken(const Token* token);

protected:
  struct {
    kSValueType type;
    union {
      int64_t t_integer;
      double t_float;
      bool t_bool;
      struct {
        const char* ptr;
        uint32_t size;
      } t_string;
    } u;
  } data_;
};

}
}
#endif
