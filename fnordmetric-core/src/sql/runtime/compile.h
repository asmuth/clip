/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_COMPILER_H
#define _FNORDMETRIC_QUERY_COMPILER_H
#include <stdlib.h>
#include <vector>
#include <string>
#include <metricd/sql/runtime/symboltable.h>

namespace fnordmetric {
namespace query {
class ASTNode;
class SValue;

enum kCompiledExpressionType {
  X_CALL,
  X_LITERAL,
  X_INPUT,
  X_MULTI
};

struct CompiledExpression {
  kCompiledExpressionType type;
  void (*call)(void*, int, SValue*, SValue*);
  void* arg0;
  CompiledExpression* next;
  CompiledExpression* child;
};

class Compiler {
public:
  Compiler(SymbolTable* symbol_table);

  CompiledExpression* compile(ASTNode* ast, size_t* scratchpad_len);

  SymbolTable* symbolTable() { return symbol_table_; }

protected:

  CompiledExpression* compileSelectList(
      ASTNode* select_list, 
      size_t* scratchpad_len);

  CompiledExpression* compileOperator(
      const std::string& name,
      ASTNode* ast,
      size_t* scratchpad_len);

  CompiledExpression* compileLiteral(ASTNode* ast);

  CompiledExpression* compileColumnReference(ASTNode* ast);

  CompiledExpression* compileChildren(ASTNode* ast, size_t* scratchpad_len);

  CompiledExpression* compileMethodCall(ASTNode* ast, size_t* scratchpad_len);

  SymbolTable* symbol_table_;
};

}
}
#endif
