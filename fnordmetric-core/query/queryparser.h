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
#include "token.h"

namespace fnordmetric {
namespace query {

class QueryParser {
  friend class QueryTest;
public:
  enum kParserErrorType {
    ERR_UNEXPECTED_TOKEN,
  };

  struct ParserError {
    kParserErrorType type;
    const char* msg;
  };

  /**
   * Parse a SQL Query from the input string. Returns true on success and false
   * if an error occurred.
   */
  size_t parse(const char* query, size_t len);

  const std::vector<ParserError>& getErrors();

protected:

  void parseSelect();
  void parseSelectList();
  void parseSelectSublist();

  bool assertExpectation(Token::kTokenType);
  void addError(kParserErrorType type, const char* msg);
  inline void consumeToken() { cur_token_++; }

  std::vector<Token> token_list_;
  std::vector<Token>::const_iterator cur_token_;
  std::vector<ParserError> errors_;
};

}
}
#endif
