/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "astnode.h"
#include "token.h"

namespace fnordmetric {
namespace query {

ASTNode::ASTNode(kASTNodeType type) : type_(type), token_(nullptr) {}

bool ASTNode::operator==(kASTNodeType type) const {
  return type_ == type;
}

ASTNode* ASTNode::appendChild(ASTNode::kASTNodeType type) {
  children_.emplace_back(type);
  // FIXPAUL slightly hacky but safe as we only recurse down the tree
  return children_.data() + (children_.size() - 1);
}

const std::vector<ASTNode>& ASTNode::getChildren() const {
  return children_;
}

void ASTNode::setToken(const Token* token) {
  token_ = token;
}

const Token* ASTNode::getToken() const {
  return token_;
}

void ASTNode::debugPrint(int indent /* = 0 */) const {
  for (int i = 0; i < indent - 2; i++) {
    printf("  ");
  }

  switch (type_) {
    case T_ALL:
      printf("- ALL\n");
      break;
    case T_ROOT:
      printf("\r");
      break;
    case T_SELECT:
      printf("- SELECT\n");
      break;
    case T_SELECT_LIST:
      printf("- SELECT_LIST\n");
      break;
    case T_FROM:
      printf("- FROM\n");
      break;
    case T_DERIVED_COLUMN:
      printf("- DERIVED_COLUMN\n");
      break;
    case T_COLUMN_NAME:
      printf("- COLUMN_NAME\n");
      break;
    case T_VALUE_EXPR:
      printf("- VALUE_EXPR\n");
      break;
  }

  for (const auto& child : children_) {
    child.debugPrint(indent + 1);
  }
}

}
}
