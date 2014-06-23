/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "compile.h"
#include "astnode.h"
#include "symboltable.h"
#include "svalue.h"

namespace fnordmetric {
namespace query {

CompiledExpression* compileAST(ASTNode* ast) {
  switch (ast->getType()) {

    case ASTNode::T_SELECT_LIST:
      return compileSelectList(ast);

    case ASTNode::T_EQ_EXPR:
      return compileOperator("eq", ast);

    case ASTNode::T_AND_EXPR:
      return compileOperator("and", ast);

    case ASTNode::T_OR_EXPR:
      return compileOperator("or", ast);

    case ASTNode::T_NEGATE_EXPR:
      return compileOperator("neg", ast);

    case ASTNode::T_LT_EXPR:
      return compileOperator("lt", ast);

    case ASTNode::T_GT_EXPR:
      return compileOperator("gt", ast);

    case ASTNode::T_ADD_EXPR:
      return compileOperator("add", ast);

    case ASTNode::T_SUB_EXPR:
      return compileOperator("sub", ast);

    case ASTNode::T_MUL_EXPR:
      return compileOperator("mul", ast);

    case ASTNode::T_DIV_EXPR:
      return compileOperator("div", ast);

    case ASTNode::T_MOD_EXPR:
      return compileOperator("mod", ast);

    case ASTNode::T_POW_EXPR:
      return compileOperator("pow", ast);

    case ASTNode::T_LITERAL:
      return compileLiteral(ast);

    case ASTNode::T_RESOLVED_COLUMN:
      return compileColumnReference(ast);
    //  assert(e->getID() >= 0);
    //  assert(cur_row_->size() > e->getID());
    //  return new SValue(*(*cur_row_)[e->getID()]);

    default:
      printf("error: cant compile expression\n");
      ast->debugPrint();
      assert(0); // FIXPAUL
  }
}


CompiledExpression* compileSelectList(ASTNode* select_list) {
  auto root = new CompiledExpression();
  root->type = X_MULTI;
  root->call = nullptr;
  root->arg0 = nullptr;
  root->next  = nullptr;

  auto cur = &root->child;
  for (auto col : select_list->getChildren()) {
    assert(*col == ASTNode::T_DERIVED_COLUMN);
    assert(col->getChildren().size() > 0);
    auto next = compileAST(col->getChildren()[0]);
    *cur = next;
    cur = &next->next;
  }

  return root;
}

CompiledExpression* compileOperator(const std::string& name, ASTNode* ast) {
  auto symbol = lookupSymbol(name);
  assert(symbol != nullptr);

  auto op = new CompiledExpression();
  op->type = X_CALL;
  op->call = symbol->getFnPtr();
  op->arg0 = nullptr;
  op->child = nullptr;
  op->next  = nullptr;

  auto cur = &op->child;
  for (auto e : ast->getChildren()) {
    auto next = compileAST(e);
    *cur = next;
    cur = &next->next;
  }

  return op;
}

CompiledExpression* compileLiteral(ASTNode* ast) {
  assert(ast->getToken() != nullptr);
  auto ins = new CompiledExpression();
  ins->type = X_LITERAL;
  ins->call = nullptr;
  ins->arg0 = SValue::fromToken(ast->getToken());
  ins->child = nullptr;
  ins->next  = nullptr;
  return ins;
}

CompiledExpression* compileColumnReference(ASTNode* ast) {
  auto ins = new CompiledExpression();
  ins->type = X_INPUT;
  ins->call = nullptr;
  ins->arg0 = (void *) ast->getID();
  ins->child = nullptr;
  ins->next  = nullptr;
  return ins;
}

}
}
