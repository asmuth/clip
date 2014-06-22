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
#include "astnode.h"

namespace fnordmetric {
namespace query {

/**
 * todo: subqueries, limit/offset, join, union
 */

class Parser {
  friend class QueryTest;
public:
  enum kParserErrorType {
    ERR_UNEXPECTED_TOKEN,
    ERR_INTERNAL_ERROR,
  };

  struct ParserError {
    kParserErrorType type;
    const char* msg;
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

  const std::vector<ParserError>& getErrors() const;
  const std::vector<ASTNode*>& getStatements() const;

  void debugPrint() const;

protected:

  void readSelect();
  void readSelectSublist(ASTNode* select_node);
  ASTNode* readValueExpression(int precedence = 0);
  ASTNode* readLHSExpression();
  ASTNode* readMethodCall();
  ASTNode* readBinaryExpression(ASTNode* lhs, int precedence);

  ASTNode* fromClause();
  ASTNode* whereClause();
  ASTNode* groupByClause();
  ASTNode* havingClause();
  ASTNode* orderByClause();
  ASTNode* limitClause();

  ASTNode* tableName();

  ASTNode* eqExpr(ASTNode* lhs, int precedence);
  ASTNode* neqExpr(ASTNode* lhs, int precedence);
  ASTNode* andExpr(ASTNode* lhs, int precedence);
  ASTNode* orExpr(ASTNode* lhs, int precedence);

  ASTNode* addExpr(ASTNode* lhs, int precedence);
  ASTNode* subExpr(ASTNode* lhs, int precedence);
  ASTNode* mulExpr(ASTNode* lhs, int precedence);
  ASTNode* divExpr(ASTNode* lhs, int precedence);
  ASTNode* modExpr(ASTNode* lhs, int precedence);
  ASTNode* powExpr(ASTNode* lhs, int precedence);

  bool assertExpectation(Token::kTokenType expectation);
  void addError(kParserErrorType type, const char* msg);

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

  inline void expectAndConsume(Token::kTokenType expectation) {
    if (assertExpectation(expectation)) {
      consumeToken();
    }
  }

  inline bool lookahead(size_t n, Token::kTokenType expectation) const {
    return cur_token_ + n < token_list_end_ &&
        cur_token_[n].getType() == expectation;
  }

  std::vector<Token> token_list_;
  Token* cur_token_;
  Token* token_list_end_;
  std::vector<ParserError> errors_;
  ASTNode root_;
};

}
}
#endif
