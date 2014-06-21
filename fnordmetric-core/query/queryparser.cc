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

QueryParser::QueryParser() : root_(ASTNode::T_ROOT) {}

size_t QueryParser::parse(const char* query, size_t len) {
  const char* cur = query;
  const char* end = cur + len;

  tokenizeQuery(&cur, end, &token_list_);
  if (token_list_.size() == 0) {
    return 0; // FIXPAUL return error...
  }

  cur_token_ = token_list_.data();
  token_list_end_ = cur_token_ + token_list_.size();
  parseSelect();

  return errors_.size() == 0;
}

void QueryParser::parseSelect() {
  if (!assertExpectation(Token::T_SELECT)) {
    return;
  } else {
    consumeToken();
  }

  // FIXPAUL parse SET_QUANTIFIER (distinct, all...)
  auto select = root_.appendChild(ASTNode::T_SELECT);
  auto select_list = select->appendChild(ASTNode::T_SELECT_LIST);

  /* select list */
  if (*cur_token_ == Token::T_ASTERISK) {
    select_list->appendChild(ASTNode::T_ALL);
    consumeToken();
  } else {
    for (;;) {
      parseSelectSublist(select_list);

      if (*cur_token_ == Token::T_COMMA) {
        consumeToken();
      } else {
        break;
      }
    }
  }

}

void QueryParser::parseSelectSublist(ASTNode* select_list) {
  /* table_name.* */
  if (cur_token_ + 3 < token_list_end_ &&
      cur_token_[0] == Token::T_STRING &&
      cur_token_[1] == Token::T_DOT &&
      cur_token_[2] == Token::T_ASTERISK) {
    //sublist->is_table_wildcard = true;
    //sublist->sublist.table_wildcard_name = cur_token_;
    return;
  }

  /* derived_col AS col_name */
  //parseValueExpression()
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

const std::vector<QueryParser::ParserError>& QueryParser::getErrors() const {
  return errors_;
}

const std::vector<ASTNode>& QueryParser::getStatements() const {
  return root_.getChildren();
}

}
}
