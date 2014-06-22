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
    T_INTEGER
  };

  explicit SValue(int64_t integer_value);
  explicit SValue(double float_value);
  explicit SValue(const char* str_value, size_t len, bool copy);
  SValue(const SValue& copy);
  SValue& operator=(const SValue& copy) = delete;

  ~SValue() {
  }

  static SValue* fromToken(const Token* token) {
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

  kSValueType getType() const {
    return type_;
  }

  int64_t getInteger() const {
    assert(type_ == T_INTEGER);
    return data_.t_integer;
  }

  std::string toString() const {
    return "blah";
  }

protected:
  kSValueType type_;
  union {
    int64_t t_integer;
    double t_float;
    struct {
      const char* begin;
      const char* end;
      bool owned;
    } t_string;
  } data_;
};

}
}
#endif
