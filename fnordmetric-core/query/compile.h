/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_COMPILE_H
#define _FNORDMETRIC_QUERY_COMPILE_H
#include <stdlib.h>
#include <vector>

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
  void (*call)(void**, int, SValue*, SValue*);
  void* arg0;
  CompiledExpression* next;
  CompiledExpression* child;
};

CompiledExpression* compileAST(ASTNode* ast);
CompiledExpression* compileSelectList(ASTNode* select_list);
CompiledExpression* compileOperator(const std::string& name, ASTNode* ast);
CompiledExpression* compileLiteral(ASTNode* ast);

}
}
#endif
