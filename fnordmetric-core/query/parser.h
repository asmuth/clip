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
  ASTNode* readValueExpression();
  ASTNode* readLHSExpression();
  ASTNode* readMethodCall();
  ASTNode* readBinaryExpression(ASTNode* lhs);

  ASTNode* addExpr(ASTNode* lhs, ASTNode* rhs);
  ASTNode* subExpr(ASTNode* lhs, ASTNode* rhs);
  ASTNode* mulExpr(ASTNode* lhs, ASTNode* rhs);
  ASTNode* divExpr(ASTNode* lhs, ASTNode* rhs);

  bool assertExpectation(Token::kTokenType expectation);
  void addError(kParserErrorType type, const char* msg);

  inline Token* consumeToken() {
    auto token = cur_token_;
    cur_token_++;
    return token;
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
