/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_QUERY_QUERYPARSER_H
#define _FNORDMETRIC_QUERY_QUERYPARSER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/sql/token.h>
#include <fnordmetric/sql/astnode.h>

namespace fnordmetric {
namespace query {

/**
 * todo: subqueries, limit/offset, join, union
 */

class Parser {
  friend class QueryTest;
public:
  struct ParseError : public fnordmetric::util::RuntimeException {
    template <typename... T>
    ParseError(
        const char* message, T... args) :
        RuntimeException(message, args...) {}
  };

  /* precedence table
    14, !
    13, - (unary minus), ~ (unary bit inversion)
    12, ^
    11, *, /, DIV, %, MOD
    10, -, +
    9, <<, >>
    8, &
    7, |
    // not implemented below here
    6, = (comparison), <=>, >=, >, <=, <, <>, !=, IS, LIKE, REGEXP, IN
    5, BETWEEN, CASE, WHEN, THEN, ELSE
    4, NOT
    3, &&, AND
    2, XOR
    1, ||, OR
  */

  Parser();

  /**
   * Parse a SQL Query from the input string. Returns true on success and false
   * if an error occurred.
   */
  size_t parse(const char* query, size_t len);

  const std::vector<ASTNode*>& getStatements() const;
  const std::vector<Token>& getTokenList() const;

  void debugPrint() const;

protected:

  ASTNode* expr(int precedence = 0);
  ASTNode* unaryExpr();
  ASTNode* binaryExpr(ASTNode* lhs, int precedence);
  ASTNode* methodCall();

  ASTNode* statement();
  ASTNode* selectStatement();
  ASTNode* selectSublist();
  ASTNode* tableName();
  ASTNode* drawStatement();
  ASTNode* axisClause();
  ASTNode* domainClause();

  ASTNode* importStatement();
  void importCSVClause(ASTNode* import);

  ASTNode* fromClause();
  ASTNode* whereClause();
  ASTNode* groupByClause();
  ASTNode* havingClause();
  ASTNode* orderByClause();
  ASTNode* limitClause();

  ASTNode* eqExpr(ASTNode* lhs, int precedence);
  ASTNode* neqExpr(ASTNode* lhs, int precedence);
  ASTNode* ltExpr(ASTNode* lhs, int precedence);
  ASTNode* gtExpr(ASTNode* lhs, int precedence);
  ASTNode* andExpr(ASTNode* lhs, int precedence);
  ASTNode* orExpr(ASTNode* lhs, int precedence);

  ASTNode* addExpr(ASTNode* lhs, int precedence);
  ASTNode* subExpr(ASTNode* lhs, int precedence);
  ASTNode* mulExpr(ASTNode* lhs, int precedence);
  ASTNode* divExpr(ASTNode* lhs, int precedence);
  ASTNode* modExpr(ASTNode* lhs, int precedence);
  ASTNode* powExpr(ASTNode* lhs, int precedence);

  bool assertExpectation(Token::kTokenType expectation);

  inline Token* consumeToken() {
    auto token = cur_token_;
    cur_token_++;
    return token;
  }

  inline bool consumeIf(Token::kTokenType expectation) {
    if (*cur_token_ == expectation) {
      consumeToken();
      return true;
    } else {
      return false;
    }
  }

  inline Token* expectAndConsume(Token::kTokenType expectation) {
    if (assertExpectation(expectation)) {
      return consumeToken();
    }

    return nullptr;
  }

  inline Token* expectAndConsume(
      const std::vector<Token::kTokenType>& expectation) {
    for (const auto& candidate : expectation) {
      if (*cur_token_ == candidate) {
        return consumeToken();
      }
    }

    std::string legal_tokens;
    for (const auto& candidate : expectation) {
      if (candidate != *expectation.begin()) {
        legal_tokens += ", ";
      }

      legal_tokens += Token::getTypeName(candidate);
    }

    RAISE(
        ParseError,
        "unexpected token %s%s%s, expected one of: %s",
        Token::getTypeName(cur_token_->getType()),
        cur_token_->getString().size() > 0 ? ": " : "",
        cur_token_->getString().c_str(),
        legal_tokens.c_str());

    return nullptr;
  }

  inline bool lookahead(size_t n, Token::kTokenType expectation) const {
    return cur_token_ + n < token_list_end_ &&
        cur_token_[n].getType() == expectation;
  }

  std::vector<Token> token_list_;
  Token* cur_token_;
  Token* token_list_end_;
  ASTNode root_;
};

}
}
#endif
