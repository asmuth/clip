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

enum kInstructionType {
  X_CALL,
  X_LITERAL,
  X_INPUT,
  X_MULTI
};

struct Instruction {
  kInstructionType type;
  SValue* (*call_)(void**, int, SValue**);
  void* arg0;
  Instruction* next;
  Instruction* child;
};

Instruction* compileAST(ASTNode* ast);
Instruction* compileSelectList(ASTNode* select_list);
Instruction* compileOperator(const std::string& name, ASTNode* ast);
Instruction* compileLiteral(ASTNode* ast);

}
}
#endif
