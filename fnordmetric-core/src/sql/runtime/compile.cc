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
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/sql/runtime/compile.h>
#include <fnordmetric/sql/runtime/symboltable.h>
#include <fnordmetric/sql/svalue.h>

namespace fnordmetric {
namespace query {

Compiler::Compiler(SymbolTable* symbol_table) : symbol_table_(symbol_table) {}

CompiledExpression* Compiler::compile(ASTNode* ast, size_t* scratchpad_len) {
  if (ast == nullptr) {
    RAISE(kNullPointerError, "can't compile nullptr");
  }

  switch (ast->getType()) {

    case ASTNode::T_SELECT_LIST:
      return compileSelectList(ast, scratchpad_len);

    case ASTNode::T_GROUP_BY:
      return compileChildren(ast, scratchpad_len);

    case ASTNode::T_EQ_EXPR:
      return compileOperator("eq", ast, scratchpad_len);

    case ASTNode::T_NEQ_EXPR:
      return compileOperator("neq", ast, scratchpad_len);

    case ASTNode::T_AND_EXPR:
      return compileOperator("and", ast, scratchpad_len);

    case ASTNode::T_OR_EXPR:
      return compileOperator("or", ast, scratchpad_len);

    case ASTNode::T_NEGATE_EXPR:
      return compileOperator("neg", ast, scratchpad_len);

    case ASTNode::T_LT_EXPR:
      return compileOperator("lt", ast, scratchpad_len);

    case ASTNode::T_LTE_EXPR:
      return compileOperator("lte", ast, scratchpad_len);

    case ASTNode::T_GT_EXPR:
      return compileOperator("gt", ast, scratchpad_len);

    case ASTNode::T_GTE_EXPR:
      return compileOperator("gte", ast, scratchpad_len);

    case ASTNode::T_ADD_EXPR:
      return compileOperator("add", ast, scratchpad_len);

    case ASTNode::T_SUB_EXPR:
      return compileOperator("sub", ast, scratchpad_len);

    case ASTNode::T_MUL_EXPR:
      return compileOperator("mul", ast, scratchpad_len);

    case ASTNode::T_DIV_EXPR:
      return compileOperator("div", ast, scratchpad_len);

    case ASTNode::T_MOD_EXPR:
      return compileOperator("mod", ast, scratchpad_len);

    case ASTNode::T_POW_EXPR:
      return compileOperator("pow", ast, scratchpad_len);

    case ASTNode::T_LITERAL:
      return compileLiteral(ast);

    case ASTNode::T_RESOLVED_COLUMN:
      return compileColumnReference(ast);

    case ASTNode::T_METHOD_CALL:
      return compileMethodCall(ast, scratchpad_len);

    default:
      ast->debugPrint();
      RAISE(kRuntimeError, "internal error: can't compile expression");
  }
}


CompiledExpression* Compiler::compileSelectList(
    ASTNode* select_list,
    size_t* scratchpad_len) {
  auto root = new CompiledExpression();
  root->type = X_MULTI;
  root->call = nullptr;
  root->arg0 = nullptr;
  root->next  = nullptr;

  auto cur = &root->child;
  for (auto col : select_list->getChildren()) {
    if (!(*col == ASTNode::T_DERIVED_COLUMN)
        || col->getChildren().size() == 0) {
      RAISE(kRuntimeError, "internal error: corrupt ast");
    }

    auto next = compile(col->getChildren()[0], scratchpad_len);
    *cur = next;
    cur = &next->next;
  }

  return root;
}

CompiledExpression* Compiler::compileChildren(
    ASTNode* parent,
    size_t* scratchpad_len) {
  auto root = new CompiledExpression();
  root->type = X_MULTI;
  root->call = nullptr;
  root->arg0 = nullptr;
  root->next  = nullptr;

  auto cur = &root->child;
  for (auto child : parent->getChildren()) {
    auto next = compile(child, scratchpad_len);
    *cur = next;
    cur = &next->next;
  }

  return root;
}

CompiledExpression* Compiler::compileOperator(
    const std::string& name,
    ASTNode* ast,
    size_t* scratchpad_len) {
  auto symbol = symbol_table_->lookupSymbol(name);

  if (symbol == nullptr) {
    RAISE(kRuntimeError, "undefined symbol: '%s'\n", name.c_str());
  }

  auto op = new CompiledExpression();
  op->type = X_CALL;
  op->call = symbol->getFnPtr();
  op->arg0 = nullptr;
  op->child = nullptr;
  op->next  = nullptr;

  auto cur = &op->child;
  for (auto e : ast->getChildren()) {
    auto next = compile(e, scratchpad_len);
    *cur = next;
    cur = &next->next;
  }

  return op;
}

CompiledExpression* Compiler::compileLiteral(ASTNode* ast) {
  if (ast->getToken() == nullptr) {
    RAISE(kRuntimeError, "internal error: corrupt ast");
  }

  auto ins = new CompiledExpression();
  ins->type = X_LITERAL;
  ins->call = nullptr;
  ins->arg0 = SValue::fromToken(ast->getToken());
  ins->child = nullptr;
  ins->next  = nullptr;

  return ins;
}

CompiledExpression* Compiler::compileColumnReference(ASTNode* ast) {
  auto ins = new CompiledExpression();
  ins->type = X_INPUT;
  ins->call = nullptr;
  ins->arg0 = (void *) ast->getID();
  ins->child = nullptr;
  ins->next  = nullptr;
  return ins;
}

CompiledExpression* Compiler::compileMethodCall(
    ASTNode* ast,
    size_t* scratchpad_len) {
  if (ast->getToken() == nullptr ||
      ast->getToken()->getType() != Token::T_IDENTIFIER) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  auto symbol = symbol_table_->lookupSymbol(ast->getToken()->getString());
  if (symbol == nullptr) {
    RAISE(
        kRuntimeError,
        "error: cannot resolve symbol: %s\n",
        ast->getToken()->getString().c_str());
  }

  auto op = new CompiledExpression();
  op->type = X_CALL;
  op->call = symbol->getFnPtr();
  op->arg0 = nullptr;
  op->child = nullptr;
  op->next  = nullptr;

  if (symbol->isAggregate()) {
    op->arg0 = (void *) *scratchpad_len;
    *scratchpad_len += symbol->getScratchpadSize();
  }

  auto cur = &op->child;
  for (auto e : ast->getChildren()) {
    auto next = compile(e, scratchpad_len);
    *cur = next;
    cur = &next->next;
  }

  return op;
}

}
}
