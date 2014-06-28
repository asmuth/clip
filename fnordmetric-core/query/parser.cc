/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "parser.h"
#include "tokenize.h"

namespace fnordmetric {
namespace query {

Parser::Parser() : root_(ASTNode::T_ROOT) {}

size_t Parser::parse(const char* query, size_t len) {
  const char* cur = query;
  const char* end = cur + len;

  tokenizeQuery(&cur, end, &token_list_);

  if (token_list_.size() == 0) {
    return 0; // FIXPAUL return error...
  }

  cur_token_ = token_list_.data();
  token_list_end_ = cur_token_ + token_list_.size();

  while (cur_token_ < token_list_end_ && errors_.size() == 0) {
    root_.appendChild(statement());
  }

  return errors_.size() == 0;
}

ASTNode* Parser::expr(int precedence /* = 0 */) {
  auto lhs = unaryExpr();

  if (lhs == nullptr) {
    return nullptr;
  }

  for (;;) {
    auto expr = binaryExpr(lhs, precedence);
    if (expr == nullptr) {
      return lhs;
    } else {
      lhs = expr;
    }
  }
}

ASTNode* Parser::unaryExpr() {
  switch (cur_token_->getType()) {

    /* parenthesized value expression */
    case Token::T_LPAREN: {
      consumeToken();
      auto e = expr();
      if (assertExpectation(Token::T_RPAREN)) {
        consumeToken();
      }
      return e;
    }

    /* prefix ~ ? */

    /* negated value expression */
    case Token::T_BANG:
    case Token::T_MINUS:
    case Token::T_NOT: {
      consumeToken();
      auto e = new ASTNode(ASTNode::T_NEGATE_EXPR);
      e->appendChild(expr());
      return e;
    }

    /* literal expression */
    case Token::T_TRUE:
    case Token::T_FALSE:
    case Token::T_NUMERIC:
    case Token::T_STRING: {
      auto e = new ASTNode(ASTNode::T_LITERAL);
      e->setToken(cur_token_);
      consumeToken();
      return e;
    }

    case Token::T_IDENTIFIER: {
      ASTNode* expr = nullptr;

      if (lookahead(1, Token::T_DOT)) {
        /* table_name.column_name */
        auto table_name = new ASTNode(ASTNode::T_TABLE_NAME);
        table_name->setToken(cur_token_);
        consumeToken();
        consumeToken();
        if (assertExpectation(Token::T_IDENTIFIER)) {
          auto col_name = table_name->appendChild(ASTNode::T_COLUMN_NAME);
          col_name->setToken(cur_token_);
          consumeToken();
        }
        return table_name;
      }

      if (lookahead(1, Token::T_LPAREN)) {
        return methodCall();
      }

      /* simple column name */
      expr = new ASTNode(ASTNode::T_COLUMN_NAME);
      expr->setToken(cur_token_);
      consumeToken();
      return expr;
    }

    default:
      return nullptr;

  }
}

ASTNode* Parser::methodCall() {
  auto e = new ASTNode(ASTNode::T_METHOD_CALL);
  e->setToken(consumeToken());

  /* read arguments */
  do {
    consumeToken();
    e->appendChild(expr());
  } while (*cur_token_ == Token::T_COMMA);

  expectAndConsume(Token::T_RPAREN);
  return e;
}

ASTNode* Parser::binaryExpr(ASTNode* lhs, int precedence) {
  switch (cur_token_->getType()) {

    /* equals expression */
    case Token::T_EQUAL:
      return eqExpr(lhs, precedence);

    /* less than expression */
    case Token::T_LT:
      return ltExpr(lhs, precedence);

    /* less than expression */
    case Token::T_GT:
      return gtExpr(lhs, precedence);

    /* and expression */
    case Token::T_AND:
      return andExpr(lhs, precedence);

    /* or expression */
    case Token::T_OR:
      return orExpr(lhs, precedence);

    /* add expression */
    case Token::T_PLUS:
      return addExpr(lhs, precedence);

    /* subtract expression */
    case Token::T_MINUS:
      return subExpr(lhs, precedence);

    /* multiply expression */
    case Token::T_ASTERISK:
      return mulExpr(lhs, precedence);

    /* division expression */
    case Token::T_SLASH:
    case Token::T_DIV:
      return divExpr(lhs, precedence);

    /* modulo expression */
    case Token::T_PERCENT:
    case Token::T_MOD:
      return modExpr(lhs, precedence);

    /* exponentiation expression */
    case Token::T_CIRCUMFLEX:
      return powExpr(lhs, precedence);

    // FIXPAUL: lshift, rshift, ampersand, pipe, tilde, noq, and, or

    default:
      return nullptr;

  }
}

ASTNode* Parser::statement() {
  switch (cur_token_->getType()) {
    case Token::T_SELECT:
      return selectStatement();
    case Token::T_SERIES:
      return seriesStatement();
    case Token::T_DRAW:
      return drawStatement();
  }

  addError(
      ERR_UNEXPECTED_TOKEN,
      "expected one of SELECT, IMPORT, SERIES or DRAW\n");
  return nullptr;
}

ASTNode* Parser::selectStatement() {
  auto select = new ASTNode(ASTNode::T_SELECT);
  consumeToken();

  /* DISTINCT/ALL */
  // FIXPAUL read SET_QUANTIFIER (distinct, all...)

  /* select list */
  auto select_list = select->appendChild(ASTNode::T_SELECT_LIST);
  if (*cur_token_ == Token::T_ASTERISK) {
    select_list->appendChild(ASTNode::T_ALL);
    consumeToken();
  } else {
    for (;;) {
      select_list->appendChild(selectSublist());

      if (*cur_token_ == Token::T_COMMA) {
        consumeToken();
      } else {
        break;
      }
    }
  }

  /* FROM clause */
  if (!(*cur_token_ == Token::T_SEMICOLON)) {
    select->appendChild(fromClause());
  }

  /* WHERE clause */
  auto where = whereClause();
  if (where != nullptr) {
    select->appendChild(where);
  }

  /* GROUP BY clause */
  auto group = groupByClause();
  if (group != nullptr) {
    select->appendChild(group);
  }

  /* HAVING clause */
  auto having = havingClause();
  if (having != nullptr) {
    select->appendChild(having);
  }

  /* ORDER BY clause */
  auto order = orderByClause();
  if (order != nullptr) {
    select->appendChild(order);
  }

  /* LIMIT clause */
  auto limit = limitClause();
  if (limit != nullptr) {
    select->appendChild(limit);
  }

  if (*cur_token_ == Token::T_SEMICOLON) {
    consumeToken();
  }

  return select;
}

ASTNode* Parser::seriesStatement() {
  auto series = new ASTNode(ASTNode::T_SERIES);
  consumeToken();

  if (lookahead(0, Token::T_STRING)) {
    auto name = series->appendChild(ASTNode::T_SERIES_NAME);
    name->setToken(consumeToken());
  } else {
    series->appendChild(expr());
  }

  if (!expectAndConsume(Token::T_FROM)) {
    return nullptr;
  }

  series->appendChild(selectStatement());
  return series;
}

ASTNode* Parser::drawStatement() {
  auto draw = new ASTNode(ASTNode::T_DRAW);
  consumeToken();

  switch (cur_token_->getType()) {
    case Token::T_BAR:
      draw->setToken(consumeToken());
      break;

    default:
      addError(
          ERR_UNEXPECTED_TOKEN,
          "expected one of BAR, LINE, AREA\n");
      return nullptr;
  }

  if (!expectAndConsume(Token::T_CHART)) {
    return nullptr;
  }

  // FIXPAUL parse params

  if (*cur_token_ == Token::T_SEMICOLON) {
    consumeToken();
  }

  return draw;
}



ASTNode* Parser::selectSublist() {
  /* table_name.* */
  if (lookahead(0, Token::T_IDENTIFIER) &&
      lookahead(1, Token::T_DOT) &&
      lookahead(2, Token::T_ASTERISK)) {
    auto select_all = new ASTNode(ASTNode::T_ALL);
    select_all->setToken(cur_token_);
    cur_token_ += 3;
    return select_all;
  }

  /* derived_col AS col_name */
  auto derived = new ASTNode(ASTNode::T_DERIVED_COLUMN);
  auto value_expr = expr();

  if (value_expr == nullptr) {
    addError(ERR_UNEXPECTED_TOKEN, "expected value expression");
    delete derived;
    return nullptr;
  }

  derived->appendChild(value_expr);

  if (consumeIf(Token::T_AS)) {
    assertExpectation(Token::T_IDENTIFIER);
  }

  if (*cur_token_ == Token::T_IDENTIFIER) {
    auto column_name = derived->appendChild(ASTNode::T_COLUMN_ALIAS);
    column_name->setToken(cur_token_);
    consumeToken();
  }

  return derived;
}

ASTNode* Parser::fromClause() {
  if (!assertExpectation(Token::T_FROM)) {
    return nullptr;
  }

  auto clause = new ASTNode(ASTNode::T_FROM);

  do {
    consumeToken();
    clause->appendChild(tableName());
  } while (*cur_token_ == Token::T_COMMA);

  return clause;
}

ASTNode* Parser::whereClause() {
  if (!consumeIf(Token::T_WHERE)) {
    return nullptr;
  }

  auto clause = new ASTNode(ASTNode::T_WHERE);
  clause->appendChild(expr());
  return clause;
}

ASTNode* Parser::groupByClause() {
  if (!consumeIf(Token::T_GROUP)) {
    return nullptr;
  }

  expectAndConsume(Token::T_BY);

  auto clause = new ASTNode(ASTNode::T_GROUP_BY);

  do {
    clause->appendChild(expr());
  } while (consumeIf(Token::T_COMMA));

  return clause;
}

ASTNode* Parser::havingClause() {
  if (!consumeIf(Token::T_HAVING)) {
    return nullptr;
  }

  auto clause = new ASTNode(ASTNode::T_HAVING);
  clause->appendChild(expr());
  return clause;
}

ASTNode* Parser::orderByClause() {
  if (!consumeIf(Token::T_ORDER)) {
    return nullptr;
  }

  expectAndConsume(Token::T_BY);

  auto clause = new ASTNode(ASTNode::T_ORDER_BY);

  do {
    auto spec = clause->appendChild(ASTNode::T_SORT_SPEC);
    spec->appendChild(expr());
    switch (cur_token_->getType()) {
      case Token::T_ASC:
      case Token::T_DESC:
        spec->setToken(consumeToken());
        break;
      default:
        break;
    }
  } while (consumeIf(Token::T_COMMA));

  return clause;
}

ASTNode* Parser::limitClause() {
  if (!consumeIf(Token::T_LIMIT)) {
    return nullptr;
  }

  ASTNode* clause;
  if (!assertExpectation(Token::T_NUMERIC)) {
    return nullptr;
  } else {
    clause = new ASTNode(ASTNode::T_LIMIT);
    clause->setToken(consumeToken());
  }

  if (consumeIf(Token::T_OFFSET)) {
    if (assertExpectation(Token::T_NUMERIC)) {
      auto offset = clause->appendChild(ASTNode::T_OFFSET);
      offset->setToken(consumeToken());
    }
  }

  return clause;
}

ASTNode* Parser::tableName() {
  if (!assertExpectation(Token::T_IDENTIFIER)) {
    return nullptr;
  }

  auto name = new ASTNode(ASTNode::T_TABLE_NAME);
  name->setToken(consumeToken());
  return name;
}

ASTNode* Parser::eqExpr(ASTNode* lhs, int precedence) {
  if (precedence < 6) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_EQ_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(6));
  return e;
}

ASTNode* Parser::ltExpr(ASTNode* lhs, int precedence) {
  if (precedence < 6) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_LT_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(6));
  return e;
}

ASTNode* Parser::gtExpr(ASTNode* lhs, int precedence) {
  if (precedence < 6) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_GT_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(6));
  return e;
}

ASTNode* Parser::andExpr(ASTNode* lhs, int precedence) {
  if (precedence < 3) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_AND_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(3));
  return e;
}

ASTNode* Parser::orExpr(ASTNode* lhs, int precedence) {
  if (precedence < 1) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_OR_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(1));
  return e;
}

ASTNode* Parser::addExpr(ASTNode* lhs, int precedence) {
  if (precedence < 10) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_ADD_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(10));
  return e;
}

ASTNode* Parser::subExpr(ASTNode* lhs, int precedence) {
  if (precedence < 10) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_SUB_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(10));
  return e;
}

ASTNode* Parser::mulExpr(ASTNode* lhs, int precedence) {
  if (precedence < 11) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_MUL_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(11));
  return e;
}

ASTNode* Parser::divExpr(ASTNode* lhs, int precedence) {
  if (precedence < 11) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_DIV_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(11));
  return e;
}

ASTNode* Parser::modExpr(ASTNode* lhs, int precedence) {
  if (precedence < 11) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_MOD_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(11));
  return e;
}

ASTNode* Parser::powExpr(ASTNode* lhs, int precedence) {
  if (precedence < 12) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto e = new ASTNode(ASTNode::T_POW_EXPR);
  e->appendChild(lhs);
  e->appendChild(expr(11));
  return e;
}

bool Parser::assertExpectation(Token::kTokenType expectation) {
  if (!(*cur_token_ == expectation)) {
    addError(ERR_UNEXPECTED_TOKEN, "unexpected token, expected ...");
    printf("got: %s, expected: %s\n",
        Token::getTypeName(cur_token_->getType()),
        Token::getTypeName(expectation));
    return false;
  }

  return true;
}

void Parser::addError(kParserErrorType type, const char* msg) {
  ParserError error;
  error.type = type;
  fprintf(stderr, "[ERROR] %s\n", msg);
  errors_.push_back(error);
}

const std::vector<Parser::ParserError>& Parser::getErrors() const {
  return errors_;
}

const std::vector<ASTNode*>& Parser::getStatements() const {
  return root_.getChildren();
}

void Parser::debugPrint() const {
  printf("[ AST ]\n");
  root_.debugPrint(2);
}


}
}
