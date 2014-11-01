/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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

    T_LITERAL,
    T_METHOD_CALL,
    T_RESOLVED_CALL,
    T_COLUMN_NAME,
    T_COLUMN_ALIAS,
    T_RESOLVED_COLUMN,
    T_TABLE_NAME,
    T_DERIVED_COLUMN,
    T_PROPERTY,
    T_PROPERTY_VALUE,

    T_SELECT,
    T_SELECT_LIST,
    T_ALL,
    T_FROM,
    T_WHERE,
    T_GROUP_BY,
    T_ORDER_BY,
    T_SORT_SPEC,
    T_HAVING,
    T_LIMIT,
    T_OFFSET,

    T_EQ_EXPR,
    T_NEQ_EXPR,
    T_LT_EXPR,
    T_LTE_EXPR,
    T_GT_EXPR,
    T_GTE_EXPR,
    T_AND_EXPR,
    T_OR_EXPR,
    T_NEGATE_EXPR,
    T_ADD_EXPR,
    T_SUB_EXPR,
    T_MUL_EXPR,
    T_DIV_EXPR,
    T_MOD_EXPR,
    T_POW_EXPR,

    T_DRAW,
    T_IMPORT,
    T_AXIS,
    T_AXIS_POSITION,
    T_AXIS_LABELS,
    T_DOMAIN,
    T_DOMAIN_SCALE,
    T_GRID,
    T_LEGEND
  };

  ASTNode(kASTNodeType type);
  bool operator==(kASTNodeType type) const;
  bool operator==(const ASTNode& other) const;
  ASTNode* appendChild(ASTNode::kASTNodeType type);
  void appendChild(ASTNode* node);
  void appendChild(ASTNode* node, size_t index);
  void removeChildByIndex(size_t index);
  void removeChildrenByType(kASTNodeType type);
  void removeChild(ASTNode* node);
  const std::vector<ASTNode*>& getChildren() const;
  void setToken(const Token* token);
  const Token* getToken() const;
  kASTNodeType getType() const;
  void setType(kASTNodeType type);
  uint64_t getID() const;
  void setID(uint64_t id);

  ASTNode* deepCopy() const;

  void debugPrint(int indent = 0) const;

protected:
  kASTNodeType type_;
  const Token* token_;
  int64_t id_;
  std::vector<ASTNode*> children_;
};


}
}
#endif
