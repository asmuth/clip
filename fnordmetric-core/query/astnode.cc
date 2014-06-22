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

ASTNode::ASTNode(kASTNodeType type) :
    type_(type),
    token_(nullptr),
    resolved_(-1) {}

bool ASTNode::operator==(kASTNodeType type) const {
  return type_ == type;
}

ASTNode* ASTNode::appendChild(ASTNode::kASTNodeType type) {
  auto child = new ASTNode(type);
  children_.push_back(child);
  return child;
}

void ASTNode::appendChild(ASTNode* node) {
  children_.push_back(node);
}

const std::vector<ASTNode*>& ASTNode::getChildren() const {
  return children_;
}

void ASTNode::setToken(const Token* token) {
  token_ = token;
}

const Token* ASTNode::getToken() const {
  return token_;
}

ASTNode::kASTNodeType ASTNode::getType() const {
  return type_;
}

void ASTNode::setType(kASTNodeType type) {
  type_ = type;
}

uint64_t ASTNode::getResolvedSymbol() const {
  return resolved_;
}

void ASTNode::setResolvedSymbol(uint64_t resolved) {
  resolved_ = resolved;
}

ASTNode* ASTNode::deepCopy() const {
  auto copy = new ASTNode(type_);

  if (token_ != nullptr) {
    copy->setToken(new Token(*token_));
  }

  for (const auto child : children_) {
    copy->appendChild(child->deepCopy());
  }

  return copy;
}

void ASTNode::debugPrint(int indent /* = 0 */) const {
  for (int i = 0; i < indent - 2; i++) {
    printf("  ");
  }

  switch (type_) {
    case T_ALL:
      printf("- ALL");
      break;
    case T_ROOT:
      printf("\r");
      break;
    case T_SELECT:
      printf("- SELECT");
      break;
    case T_SELECT_LIST:
      printf("- SELECT_LIST");
      break;
    case T_FROM:
      printf("- FROM");
      break;
    case T_WHERE:
      printf("- WHERE");
      break;
    case T_GROUP_BY:
      printf("- GROUP_BY");
      break;
    case T_ORDER_BY:
      printf("- ORDER_BY");
      break;
    case T_LIMIT:
      printf("- LIMIT");
      break;
    case T_OFFSET:
      printf("- OFFSET");
      break;
    case T_HAVING:
      printf("- HAVING");
      break;
    case T_SORT_SPEC:
      printf("- SORT_SPEC");
      break;
    case T_DERIVED_COLUMN:
      printf("- DERIVED_COLUMN");
      break;
    case T_COLUMN_NAME:
      printf("- COLUMN_NAME");
      break;
    case T_TABLE_NAME:
      printf("- TABLE_NAME");
      break;
    case T_LITERAL:
      printf("- LITERAL");
      break;
    case T_NEGATE_EXPR:
      printf("- NEGATE_EXPR");
      break;
    case T_EQ_EXPR:
      printf("- EQ_EXPR");
      break;
    case T_LT_EXPR:
      printf("- LT_EXPR");
      break;
    case T_GT_EXPR:
      printf("- GT_EXPR");
      break;
    case T_NEQ_EXPR:
      printf("- NEQ_EXPR");
      break;
    case T_AND_EXPR:
      printf("- AND_EXPR");
      break;
    case T_OR_EXPR:
      printf("- OR_EXPR");
      break;
    case T_ADD_EXPR:
      printf("- ADD_EXPR");
      break;
    case T_SUB_EXPR:
      printf("- SUB_EXPR");
      break;
    case T_MUL_EXPR:
      printf("- MUL_EXPR");
      break;
    case T_DIV_EXPR:
      printf("- DIV_EXPR");
      break;
    case T_MOD_EXPR:
      printf("- MOD_EXPR");
      break;
    case T_POW_EXPR:
      printf("- POW_EXPR");
      break;
    case T_METHOD_CALL:
      printf("- METHOD_CALL");
      break;
  }

  if (token_ != nullptr) {
    printf(" [%s]", Token::getTypeName(token_->getType()));
  }
  printf("\n");

  for (const auto child : children_) {
    child->debugPrint(indent + 1);
  }
}

}
}
