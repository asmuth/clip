/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <assert.h>
#include "astnode.h"
#include "token.h"
#include <fnord/base/inspect.h>

namespace fnordmetric {
namespace query {

ASTNode::ASTNode(kASTNodeType type) :
    type_(type),
    token_(nullptr),
    id_(-1) {}

bool ASTNode::operator==(kASTNodeType type) const {
  return type_ == type;
}

bool ASTNode::operator==(const ASTNode& other) const {
  if (type_ != other.type_) {
    return false;
  }

  if (token_ == nullptr) {
    return other.token_ == nullptr;
  } else {
    return *token_ == *other.token_;
  }
}

ASTNode* ASTNode::appendChild(ASTNode::kASTNodeType type) {
  auto child = new ASTNode(type);
  children_.push_back(child);
  return child;
}

void ASTNode::removeChildByIndex(size_t index) {
  children_.erase(children_.begin() + index);
}

void ASTNode::removeChild(ASTNode* node) {
  for (auto child = children_.begin(); child < children_.end(); ++child) {
    if (*child == node) {
      children_.erase(child);
      return;
    }
  }
}

void ASTNode::removeChildrenByType(kASTNodeType type) {
  for (auto child = children_.begin(); child < children_.end(); ++child) {
    if ((*child)->getType() == type) {
      children_.erase(child);
    }
  }
}

void ASTNode::appendChild(ASTNode* node) {
  children_.push_back(node);
}

void ASTNode::appendChild(ASTNode* node, size_t index) {
  children_.insert(children_.begin() + index, node);
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

uint64_t ASTNode::getID() const {
  return id_;
}

void ASTNode::setID(uint64_t id) {
  id_ = id;
}

ASTNode* ASTNode::deepCopy() const {
  auto copy = new ASTNode(type_);

  if (token_ != nullptr) {
    copy->setToken(new Token(*token_));
  }

  for (const auto child : children_) {
    if (child == nullptr) {
      continue;
    }

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
    case T_RESOLVED_COLUMN:
      printf("- RESOLVED_COLUMN");
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
      printf("- <method_call>");
      break;
    default:
      printf("- <unknown ASTNode>");
      break;
  }

  if (token_ != nullptr) {
    printf(
        " [%s] (%s)",
        Token::getTypeName(token_->getType()),
        token_->getString().c_str());
  }
  printf("\n");

  for (const auto child : children_) {
    if (child == nullptr) {

      for (int i = 0; i < indent - 1; i++) {
        printf("  ");
      }

      printf("- <nullptr>\n");
    } else {
      child->debugPrint(indent + 1);
    }
  }
}

} // namespace query
} // namespace fnordmetric

namespace fnord {

template <>
std::string inspect<
    fnordmetric::query::ASTNode::kASTNodeType>(
    const fnordmetric::query::ASTNode::kASTNodeType& value) {
  return "<ASTNode>";
}

template <>
std::string inspect<
    fnordmetric::query::ASTNode>(
    const fnordmetric::query::ASTNode& value) {
  return "<ASTNode>";
}

} // namespace fnord
