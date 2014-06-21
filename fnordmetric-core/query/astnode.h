/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_ASTNODE_H
#define _FNORDMETRIC_QUERY_ASTNODE_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace query {
class Token;

class ASTNode {
  friend class QueryTest;
public:
  enum kASTNodeType {
    T_ROOT,
    T_SELECT,
    T_SELECT_LIST,
    T_ALL,
    T_DERIVED_COLUMN,
    T_FROM,
    T_COLUMN_NAME,
    T_TABLE_NAME,
    T_VALUE_EXPR,
    T_NEGATE_EXPR,
    T_ADD_EXPR,
    T_SUB_EXPR,
    T_MUL_EXPR,
    T_DIV_EXPR,
    T_LITERAL,
    T_METHOD_CALL,
    T_MOD_EXPR,
    T_POW_EXPR,
  };

  ASTNode(kASTNodeType type);
  bool operator==(kASTNodeType type) const;
  ASTNode* appendChild(ASTNode::kASTNodeType type);
  void appendChild(ASTNode* node);
  const std::vector<ASTNode*>& getChildren() const;
  void setToken(const Token* token);
  const Token* getToken() const;
  void debugPrint(int indent = 0) const;

protected:
  const kASTNodeType type_;
  const Token* token_;
  std::vector<ASTNode*> children_;
};


}
}
#endif
