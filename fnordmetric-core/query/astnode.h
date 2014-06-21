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
    T_FROM,
  };

  ASTNode(kASTNodeType type);
  bool operator==(kASTNodeType type) const;
  ASTNode* appendChild(ASTNode::kASTNodeType type);
  const std::vector<ASTNode>& getChildren() const;
  void setToken(const Token* token);
  const Token* getToken() const;
  void debugPrint() const;

protected:
  const kASTNodeType type_;
  const Token* token_;
  std::vector<ASTNode> children_;
};


}
}
#endif
