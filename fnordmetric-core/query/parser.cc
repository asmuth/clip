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
  readSelect();

  return errors_.size() == 0;
}

ASTNode* Parser::readValueExpression(int precedence /* = 0 */) {
  auto lhs = readLHSExpression();

  if (lhs == nullptr) {
    return nullptr;
  }

  for (;;) {
    auto expr = readBinaryExpression(lhs, precedence);
    if (expr == nullptr) {
      return lhs;
    } else {
      lhs = expr;
    }
  }
}

ASTNode* Parser::readLHSExpression() {
  switch (cur_token_->getType()) {

    /* parenthesized value expression */
    case Token::T_LPAREN: {
      consumeToken();
      auto expr = readValueExpression();
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
      consumeToken();
      auto expr = new ASTNode(ASTNode::T_NEGATE_EXPR);
      expr->appendChild(readValueExpression());
      return expr;
    }

    /* literal expression */
    case Token::T_TRUE:
    case Token::T_FALSE:
    case Token::T_NUMERIC:
    case Token::T_STRING: {
      auto expr = new ASTNode(ASTNode::T_LITERAL);
      expr->setToken(cur_token_);
      consumeToken();
      return expr;
    }

    case Token::T_IDENTIFIER: {
      ASTNode* expr = nullptr;

      if (cur_token_ + 1 < token_list_end_ &&
          cur_token_[1].getType() == Token::T_DOT) {
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
        return readMethodCall();
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

ASTNode* Parser::readMethodCall() {
  auto expr = new ASTNode(ASTNode::T_METHOD_CALL);
  expr->setToken(consumeToken());

  /* read arguments */
  do {
    consumeToken();
    expr->appendChild(readValueExpression());
  } while (*cur_token_ == Token::T_COMMA);

  expectAndConsume(Token::T_RPAREN);
  return expr;
}

ASTNode* Parser::readBinaryExpression(ASTNode* lhs, int precedence) {
  switch (cur_token_->getType()) {

    /* euqals expression */
    case Token::T_EQUAL:
      return eqExpr(lhs, precedence);

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

void Parser::readSelect() {
  /* SELECT */
  auto select = root_.appendChild(ASTNode::T_SELECT);
  if (!assertExpectation(Token::T_SELECT)) {
    return;
  } else {
    consumeToken();
  }

  /* DISTINCT/ALL */
  // FIXPAUL read SET_QUANTIFIER (distinct, all...)

  /* select list */
  auto select_list = select->appendChild(ASTNode::T_SELECT_LIST);
  if (*cur_token_ == Token::T_ASTERISK) {
    select_list->appendChild(ASTNode::T_ALL);
    consumeToken();
  } else {
    for (;;) {
      readSelectSublist(select_list);

      if (*cur_token_ == Token::T_COMMA) {
        consumeToken();
      } else {
        break;
      }
    }
  }

  if (*cur_token_ == Token::T_SEMICOLON) {
    return;
  }

  /* FROM clause */
  select->appendChild(fromClause());

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

  /* ORDER BY */
  auto order = orderByClause();
  if (order != nullptr) {
    select->appendChild(order);
  }
}

void Parser::readSelectSublist(ASTNode* select_list) {
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
  auto value_expr = readValueExpression();

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
  clause->appendChild(readValueExpression());
  return clause;
}

ASTNode* Parser::groupByClause() {
  if (!consumeIf(Token::T_GROUP)) {
    return nullptr;
  }

  expectAndConsume(Token::T_BY);

  auto clause = new ASTNode(ASTNode::T_GROUP_BY);

  do {
    clause->appendChild(readValueExpression());
  } while (consumeIf(Token::T_COMMA));

  return clause;
}

ASTNode* Parser::havingClause() {
  if (!consumeIf(Token::T_HAVING)) {
    return nullptr;
  }

  auto clause = new ASTNode(ASTNode::T_HAVING);
  clause->appendChild(readValueExpression());
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
    spec->appendChild(readValueExpression());
    switch (cur_token_->getType()) {
      case Token::T_ASC:
      case Token::T_DESC:
        spec->setToken(consumeToken());
        break;
      default:
        printf("!!!\n");
        return nullptr; // fixpaul add error and free clause
    }
  } while (consumeIf(Token::T_COMMA));

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

  auto expr = new ASTNode(ASTNode::T_EQ_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(6));
  return expr;
}

ASTNode* Parser::andExpr(ASTNode* lhs, int precedence) {
  if (precedence < 3) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto expr = new ASTNode(ASTNode::T_AND_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(3));
  return expr;
}

ASTNode* Parser::orExpr(ASTNode* lhs, int precedence) {
  if (precedence < 1) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto expr = new ASTNode(ASTNode::T_OR_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(1));
  return expr;
}

ASTNode* Parser::addExpr(ASTNode* lhs, int precedence) {
  if (precedence < 10) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto expr = new ASTNode(ASTNode::T_ADD_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(10));
  return expr;
}

ASTNode* Parser::subExpr(ASTNode* lhs, int precedence) {
  if (precedence < 10) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto expr = new ASTNode(ASTNode::T_SUB_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(10));
  return expr;
}

ASTNode* Parser::mulExpr(ASTNode* lhs, int precedence) {
  if (precedence < 11) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto expr = new ASTNode(ASTNode::T_MUL_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(11));
  return expr;
}

ASTNode* Parser::divExpr(ASTNode* lhs, int precedence) {
  if (precedence < 11) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto expr = new ASTNode(ASTNode::T_DIV_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(11));
  return expr;
}

ASTNode* Parser::modExpr(ASTNode* lhs, int precedence) {
  if (precedence < 11) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto expr = new ASTNode(ASTNode::T_MOD_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(11));
  return expr;
}

ASTNode* Parser::powExpr(ASTNode* lhs, int precedence) {
  if (precedence < 12) {
    consumeToken();
  } else {
    return nullptr;
  }

  auto expr = new ASTNode(ASTNode::T_POW_EXPR);
  expr->appendChild(lhs);
  expr->appendChild(readValueExpression(11));
  return expr;
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
