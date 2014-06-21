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
  if (token_list_.size() == 0) {
    return 0; // FIXPAUL return error...
  }

  cur_token_ = token_list_.begin();
  statements_.push_back(parseSelect());

  return errors_.size() == 0;
}

std::unique_ptr<SelectASTNode> QueryParser::parseSelect() {
  assertExpectation(Token::T_SELECT);
  consumeToken();

  // FIXPAUL parse SET_QUANTIFIER (distinct, all...)
  std::unique_ptr<SelectASTNode> node(new SelectASTNode());
  node->select_lists.push_back(parseSelectList());

  return node;
}

std::unique_ptr<SelectListASTNode> QueryParser::parseSelectList() {
  std::unique_ptr<SelectListASTNode> node(new SelectListASTNode());

  if (*cur_token_ == Token::T_ASTERISK) {
    node->is_wildcard = true;
    return node;
  } else {
    node->is_wildcard = false;
  }

  for (;;) {
    node->select_sublists.push_back(parseSelectSublist());

    if (*cur_token_ == Token::T_COMMA) {
      consumeToken();
    } else {
      break;
    }
  }

  return node;
}

std::unique_ptr<SelectSublistASTNode> QueryParser::parseSelectSublist() {
  std::unique_ptr<SelectSublistASTNode> sublist(new SelectSublistASTNode());

  /* table_name.* */
  if (cur_token_ + 3 < token_list_.end() &&
      cur_token_[0] == Token::T_STRING &&
      cur_token_[1] == Token::T_DOT &&
      cur_token_[2] == Token::T_ASTERISK) {
    sublist->is_wildcard = true;
    //sublist->wildcard.table_name = cur_token_->getPtr(); // HACK !!! ;)
    return sublist;
  }

  /* derived_col AS col_name */
  {
    return sublist;
  }
}

bool QueryParser::assertExpectation(Token::kTokenType expectation) {
  if (!(*cur_token_ == expectation)) {
    addError(ERR_UNEXPECTED_TOKEN, "unexpected token, expected T_SELECT");
    return false;
  }

  return true;
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
