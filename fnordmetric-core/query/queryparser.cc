/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "queryparser.h"
#include "tokenize.h"

namespace fnordmetric {
namespace query {

size_t QueryParser::parse(const char* query, size_t len) {
  const char* cur = query;
  const char* end = cur + len;

  tokenizeQuery(&cur, end, &token_list_);

  for (const auto& token : token_list_) {
    //token.debugPrint();
  }

  if (token_list_.size() == 0) {
    return 0; // FIXPAUL return error...
  }

  cur_token_ = token_list_.begin();
  parseSelect();

  return errors_.size() == 0;
}

void QueryParser::parseSelect() {
  assertExpectation(Token::T_SELECT);
  consumeToken();

  // FIXPAUL parse SET_QUANTIFIER (distinct, all...)

  parseSelectList();
}

void QueryParser::parseSelectList() {
  if (*cur_token_ == Token::T_ASTERISK) {
    return; // AsteriskSelectListAstNode
  }

next_sublist:
  parseSelectSublist();

  if (*cur_token_ == Token::T_COMMA) {
    consumeToken();
    goto next_sublist;
  }
}

void QueryParser::parseSelectSublist() {

}

bool QueryParser::assertExpectation(Token::kTokenType expectation) {
  if (!(*cur_token_ == expectation)) {
    addError(ERR_UNEXPECTED_TOKEN, "unexpected token, expected T_SELECT");
  }
}

void QueryParser::addError(kParserErrorType type, const char* msg) {
  ParserError error;
  error.type = type;
  errors_.push_back(error);
}

const std::vector<QueryParser::ParserError>& QueryParser::getErrors() {
  return errors_;
}

}
}
