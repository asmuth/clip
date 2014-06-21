/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_TOKEN_H
#define _FNORDMETRIC_QUERY_TOKEN_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace query {

class Token {
  friend class QueryTest;
public:
  enum kTokenType {
    T_SELECT,
    T_FROM,
    T_COMMA,
    T_DOT,
    T_STRING,
    T_SEMICOLON,
    T_LPAREN,
    T_RPAREN,
    T_EQUAL,
    T_PLUS,
    T_ASTERISK,
    T_AS
  };

  Token(kTokenType token_type);
  Token(kTokenType token_type, const char* data, size_t size);
  Token(const Token& copy);
  ~Token();

  Token& operator=(const Token& copy) = delete;
  bool operator==(const std::string& string) const;
  bool operator==(kTokenType type) const;
  void debugPrint() const;

protected:
  char* const data_;
  const size_t len_;
  const kTokenType type_;
};

}
}
#endif
