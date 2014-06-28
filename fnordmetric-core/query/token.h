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
    T_WHERE,
    T_GROUP,
    T_ORDER,
    T_BY,
    T_HAVING,
    T_LIMIT,
    T_OFFSET,
    T_ASC,
    T_DESC,
    T_COMMA,
    T_DOT,
    T_IDENTIFIER,
    T_STRING,
    T_NUMERIC,
    T_SEMICOLON,
    T_LPAREN,
    T_RPAREN,
    T_AND,
    T_OR,
    T_EQUAL,
    T_PLUS,
    T_MINUS,
    T_ASTERISK,
    T_SLASH,
    T_AS,
    T_NOT,
    T_TRUE,
    T_FALSE,
    T_BANG,
    T_CIRCUMFLEX,
    T_TILDE,
    T_PERCENT,
    T_DIV,
    T_MOD,
    T_AMPERSAND,
    T_PIPE,
    T_LSHIFT,
    T_RSHIFT,
    T_LT,
    T_GT,
    T_SERIES,
    T_DRAW,
    T_BAR,
    T_LINE,
    T_AREA,
    T_CHART
  };

  Token(kTokenType token_type);
  Token(kTokenType token_type, const char* data, size_t size);
  Token(const Token& copy);
  ~Token();
  Token& operator=(const Token& copy) = delete;
  bool operator==(const Token& other) const;
  bool operator==(const std::string& string) const;
  bool operator==(kTokenType type) const;
  kTokenType getType() const;
  static const char* getTypeName(kTokenType type);

  const std::string getString() const;
  int64_t getInteger() const;
  bool isDouble() const;
  double getDouble() const;

  void debugPrint() const;

protected:
  char* const data_;
  const size_t len_;
  const kTokenType type_;
};

}
}
#endif
