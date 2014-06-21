/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "astnode.h"

namespace fnordmetric {
namespace query {

ASTNode::ASTNode(kASTNodeType type) : type_(type) {}

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

void ASTNode::debugPrint() const {

}

}
}
