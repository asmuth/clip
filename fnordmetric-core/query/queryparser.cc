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
  /* SELECT */
  auto select = root_.appendChild(ASTNode::T_SELECT);
  if (!assertExpectation(Token::T_SELECT)) {
    return;
  } else {
    consumeToken();
  }

  /* DISTINCT/ALL */
  // FIXPAUL parse SET_QUANTIFIER (distinct, all...)

  /* select list */
  auto select_list = select->appendChild(ASTNode::T_SELECT_LIST);
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

  /* FROM */
  auto from = select->appendChild(ASTNode::T_FROM);
  if (!assertExpectation(Token::T_FROM)) {
    return;
  } else {
    consumeToken();
  }

}

void QueryParser::parseSelectSublist(ASTNode* select_list) {
  /* table_name.* */
  if (cur_token_ + 3 < token_list_end_ &&
      cur_token_[0] == Token::T_IDENTIFIER &&
      cur_token_[1] == Token::T_DOT &&
      cur_token_[2] == Token::T_ASTERISK) {
    auto select_all = select_list->appendChild(ASTNode::T_ALL);
    select_all->setToken(cur_token_);
    cur_token_ += 3;
    return;
  }

  /* derived_col AS col_name */
  auto derived = select_list->appendChild(ASTNode::T_DERIVED_COLUMN);
  auto value_expr = parseValueExpression();

  if (value_expr == nullptr) {
    addError(ERR_UNEXPECTED_TOKEN, "expected value expression");
    // free value_expr
    return;
  }

  derived->appendChild(value_expr);

  if (*cur_token_ == Token::T_AS) {
    consumeToken();
    if (assertExpectation(Token::T_IDENTIFIER)) {
      auto column_name = derived->appendChild(ASTNode::T_COLUMN_NAME);
      column_name->setToken(cur_token_);
      consumeToken();
    }
  }
}


ASTNode* QueryParser::parseValueExpression() {
  auto lhs = parsePrefixOpExpression();

  if (lhs == nullptr) {
    return nullptr;
  }

  for (;;) {
    auto op = parseInfixOperator();
    if (op == nullptr) {
      return lhs;
    } else {
      lhs = parseInfixOpExpression(lhs, op);
    }
  }
}

ASTNode* QueryParser::parsePrefixOpExpression() {
  printf("parsePrexixOpExpr\n");
  cur_token_->debugPrint();

  switch (cur_token_->getType()) {

    /* parenthesized value expression */
    case Token::T_LPAREN: {
      consumeToken();
      auto expr = parseValueExpression();
      if (assertExpectation(Token::T_RPAREN)) {
        consumeToken();
      }
      return expr;
    }

    /* prefix ~ ? */

    /* negated value expression */
    case Token::T_BANG:
    case Token::T_MINUS:
    case Token::T_NOT: {
      printf("negate..\n");
      auto expr = new ASTNode(ASTNode::T_NEGATE_EXPR);
      expr->appendChild(parseValueExpression());
      consumeToken();
      return expr;
    }

    case Token::T_NUMERIC: {
      auto expr = new ASTNode(ASTNode::T_LITERAL);
      expr->setToken(cur_token_);
      consumeToken();
      return expr;
    }

    default:
      return nullptr;

  }
}
Token* QueryParser::parseInfixOperator() {
  printf("parseInfixOperator\n");
  cur_token_->debugPrint();

  switch (cur_token_->getType()) {

    case Token::T_MINUS:
    case Token::T_PLUS: {
      auto op = cur_token_;
      consumeToken();
      return op;
    }

    default:
      return nullptr;
  }
}

ASTNode* QueryParser::parseInfixOpExpression(ASTNode* lhs, Token* op) {
  auto rhs = parseValueExpression();

  switch (op->getType()) {

    case Token::T_PLUS: {
      auto expr = new ASTNode(ASTNode::T_ADD_EXPR);
      expr->appendChild(lhs);
      expr->appendChild(rhs);
      return expr;
    }

    /* we should never get here unless a case is missing in the switch */
    default: {
      addError(ERR_INTERNAL_ERROR, "internal error while parsing the query in"
          "QueryParser::parseInfixOpExpression");
      return nullptr;
    }

  }
}

bool QueryParser::assertExpectation(Token::kTokenType expectation) {
  if (!(*cur_token_ == expectation)) {
    addError(ERR_UNEXPECTED_TOKEN, "unexpected token, expected ...");
    return false;
  }

  return true;
}

void QueryParser::addError(kParserErrorType type, const char* msg) {
  ParserError error;
  error.type = type;
  fprintf(stderr, "[ERROR] %s\n", msg);
  errors_.push_back(error);
}

const std::vector<QueryParser::ParserError>& QueryParser::getErrors() const {
  return errors_;
}

const std::vector<ASTNode*>& QueryParser::getStatements() const {
  return root_.getChildren();
}

void QueryParser::debugPrint() const {
  printf("[ AST ]\n");
  root_.debugPrint(2);
}


}
}
