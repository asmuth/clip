/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_QUERYPARSER_H
#define _FNORDMETRIC_QUERY_QUERYPARSER_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace query {

class QueryParser {
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
    T_PLUS
  };

  struct Token {
    Token(kTokenType token_type, const char* token_addr);
    kTokenType type;
    const char* addr;
    size_t len;
    void debugPrint() const;
    bool operator==(const std::string& string) const;
  };

  /**
   * Parse a single SQL Query from the input string. Returns the number of bytes
   * that have been consumed.
   */
  size_t parse(const char* query, size_t len);

  size_t tokenize(
      const char* query,
      size_t len,
      std::vector<Token>* token_list);

protected:

  void tokenize(
      const char** cur,
      const char* end,
      std::vector<Token>* token_list);

  std::vector<Token> token_list_;
};

}
}
#endif
