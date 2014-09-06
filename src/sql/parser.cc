/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <assert.h>
#include "parser.h"
#include "tokenize.h"

namespace fnordmetric {
namespace query {

Parser::Parser() : root_(ASTNode::T_ROOT) {}

// FIXPAUL free all explicit news on ex!

size_t Parser::parse(const char* query, size_t len) {
  const char* cur = query;
  const char* end = cur + len;

  tokenizeQuery(&cur, end, &token_list_);

  if (token_list_.size() == 0) {
    return 0; // FIXPAUL return error...
  }

  cur_token_ = token_list_.data();
  token_list_end_ = cur_token_ + token_list_.size();

  while (cur_token_ < token_list_end_) {
    root_.appendChild(statement());
  }

  return true;
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
    case Token::T_DRAW:
      return drawStatement();
    case Token::T_IMPORT:
      return importStatement();
  }

  RAISE(
      ParseError,
      "unexpected token %s%s%s, expected one of SELECT, DRAW or IMPORT",
        Token::getTypeName(cur_token_->getType()),
        cur_token_->getString().size() > 0 ? ": " : "",
        cur_token_->getString().c_str());

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


ASTNode* Parser::importStatement() {
  auto import = new ASTNode(ASTNode::T_IMPORT);
  consumeToken();

  expectAndConsume(Token::T_TABLE);
  import->appendChild(tableName());
  expectAndConsume(Token::T_FROM);

  switch (cur_token_->getType()) {
    case Token::T_CSV:
      importCSVClause(import);
      break;

    default:
      RAISE(
          ParseError,
          "unexpected token %s%s%s, expected one of CSV, MYSQL",
          Token::getTypeName(cur_token_->getType()),
          cur_token_->getString().size() > 0 ? ": " : "",
          cur_token_->getString().c_str());
  }

  consumeIf(Token::T_SEMICOLON);
  return import;
}

void Parser::importCSVClause(ASTNode* import) {
  import->setToken(consumeToken());
  assertExpectation(Token::T_STRING);

  auto filename = new ASTNode(ASTNode::T_LITERAL);
  filename->setToken(consumeToken());
  import->appendChild(filename);

  for (;;) {
    switch (cur_token_->getType()) {

      case Token::T_HEADER: {
        auto flag = new ASTNode(ASTNode::T_PROPERTY);
        flag->setToken(consumeToken());
        import->appendChild(flag);
        continue;
      }

      default:
        break;
    }

    break;
  }
}

// FIXPAUL move this into sql extensions
ASTNode* Parser::drawStatement() {
  auto chart = new ASTNode(ASTNode::T_DRAW);
  consumeToken();

  chart->setToken(expectAndConsume(std::vector<Token::kTokenType>{
      Token::T_BARCHART,
      Token::T_LINECHART}));

  consumeIf(Token::T_WITH);

  while (cur_token_->getType() != Token::T_SEMICOLON) {
    switch (cur_token_->getType()) {
      case Token::T_AXIS:
        chart->appendChild(axisClause());
        break;

      case Token::T_XDOMAIN:
      case Token::T_YDOMAIN:
      case Token::T_ZDOMAIN:
        chart->appendChild(domainClause());
        break;

      case Token::T_GRID: {
        auto grid = chart->appendChild(ASTNode::T_GRID);
        consumeToken();

        for (int i = 0; i < 2; i++) {
          switch (cur_token_->getType()) {
            case Token::T_HORIZONTAL:
            case Token::T_VERTICAL: {
              auto prop = grid->appendChild(ASTNode::T_PROPERTY);
              prop->setToken(consumeToken());
              break;
            }
            default:
              break;
          }
        }

        break;
      }

      case Token::T_ORIENTATION: {
        auto prop = chart->appendChild(ASTNode::T_PROPERTY);
        prop->setToken(consumeToken());
        prop->appendChild(ASTNode::T_PROPERTY_VALUE)->setToken(
            expectAndConsume(std::vector<Token::kTokenType>{
                Token::T_HORIZONTAL,
                Token::T_VERTICAL}));
        break;
      }

      case Token::T_STACKED: {
        auto prop = chart->appendChild(ASTNode::T_PROPERTY);
        prop->setToken(consumeToken());
        prop->appendChild(ASTNode::T_PROPERTY_VALUE);
        break;
      }

      case Token::T_TITLE:
      case Token::T_SUBTITLE: {
        auto prop = chart->appendChild(ASTNode::T_PROPERTY);
        prop->setToken(consumeToken());
        prop->appendChild(expectAndConsumeValueExpr());
        break;
      }

      default:
        RAISE(
            ParseError,
            "unexpected token %s%s%s",
            Token::getTypeName(cur_token_->getType()),
            cur_token_->getString().size() > 0 ? ": " : "",
            cur_token_->getString().c_str());
        return nullptr;
    }
  }

  consumeIf(Token::T_SEMICOLON);
  return chart;
}

ASTNode* Parser::axisClause() {
  auto axis = new ASTNode(ASTNode::T_AXIS);
  axis->setToken(consumeToken());

  switch (cur_token_->getType()) {
    case Token::T_TOP:
    case Token::T_RIGHT:
    case Token::T_BOTTOM:
    case Token::T_LEFT:
      axis->appendChild(ASTNode::T_AXIS_POSITION)->setToken(consumeToken());
      break;

    default:
      RAISE(
          ParseError,
          "unexpected token %s%s%s, expected one of TOP, RIGHT, BOTTOM, LEFT",
          Token::getTypeName(cur_token_->getType()),
          cur_token_->getString().size() > 0 ? ": " : "",
          cur_token_->getString().c_str());
      return nullptr;
  }

  while (cur_token_->getType() != Token::T_SEMICOLON) {
    switch (cur_token_->getType()) {
      case Token::T_TITLE: {
        auto title = axis->appendChild(ASTNode::T_PROPERTY);
        title->setToken(consumeToken());
        title->appendChild(expectAndConsumeValueExpr());
        continue;
      }

      default:
        break;

    }

    break;
  }

  return axis;
}

ASTNode* Parser::domainClause() {
  auto domain = new ASTNode(ASTNode::T_DOMAIN);
  domain->setToken(consumeToken());

  auto min_expr = expr();

  if (min_expr != nullptr) {
    expectAndConsume(Token::T_COMMA);
    auto scale = domain->appendChild(ASTNode::T_DOMAIN_SCALE);
    scale->appendChild(min_expr);
    scale->appendChild(expectAndConsumeValueExpr());
  }

  for (int i = 0; i < 2; i++) {
    switch (cur_token_->getType()) {
      case Token::T_INVERT:
      case Token::T_LOGARITHMIC: {
        auto prop = domain->appendChild(ASTNode::T_PROPERTY);
        prop->setToken(consumeToken());
        prop->appendChild(ASTNode::T_PROPERTY_VALUE);
        continue;
      }

      default:
        break;
    }

    break;
  }

  return domain;
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
  auto derived = new ASTNode(ASTNode::T_DERIVED_COLUMN); // FIXPAUL free on ex
  derived->appendChild(expectAndConsumeValueExpr());

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
    RAISE(
        ParseError,
        "unexpected token %s%s%s, expected: '%s'",
        Token::getTypeName(cur_token_->getType()),
        cur_token_->getString().size() > 0 ? ": " : "",
        cur_token_->getString().c_str(),
        Token::getTypeName(expectation));
    return false;
  }

  return true;
}

const std::vector<ASTNode*>& Parser::getStatements() const {
  return root_.getChildren();
}

const std::vector<Token>& Parser::getTokenList() const {
  return token_list_;
}

void Parser::debugPrint() const {
  printf("[ AST ]\n");
  root_.debugPrint(2);
}


}
}
