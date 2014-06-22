/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_EXECUTABLE_H
#define _FNORDMETRIC_QUERY_EXECUTABLE_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"
#include "svalue.h"

namespace fnordmetric {
namespace query {

class RowSink {
public:
  virtual bool nextRow(std::vector<SValue*> row) = 0;
};

class Executable : public RowSink {
public:
  Executable();
  virtual ~Executable();
  virtual void execute() = 0;

  virtual size_t getNumCols() const = 0;
  virtual const std::vector<std::string>& getColumns() const = 0;

  void setTarget(RowSink* target) {
    target_ = target;
  }

protected:

  void setCurrentRow(std::vector<SValue*>* row) {
    cur_row_ = row;
  }

  bool emitRow(std::vector<SValue*> row) {
    assert(target_ != nullptr);
    return target_->nextRow(row);
  }

  SValue* expr(ASTNode* e);
  SValue* eqExpr(ASTNode* e);
  SValue* andExpr(ASTNode* e);
  SValue* orExpr(ASTNode* e);
  SValue* negExpr(ASTNode* e);
  SValue* gtExpr(ASTNode* e);
  SValue* ltExpr(ASTNode* e);
  SValue* addExpr(ASTNode* e);
  SValue* subExpr(ASTNode* e);
  SValue* mulExpr(ASTNode* e);
  SValue* divExpr(ASTNode* e);
  SValue* modExpr(ASTNode* e);
  SValue* powExpr(ASTNode* e);

  RowSink* target_;
  std::vector<SValue*>* cur_row_;
};

}
}
#endif
