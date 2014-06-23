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
  virtual bool nextRow(SValue* row, int row_len) = 0;
};

class Executable : public RowSink {
public:
  Executable();
  virtual ~Executable();

  virtual void execute() = 0;

  virtual size_t getNumCols() const = 0;
  virtual const std::vector<std::string>& getColumns() const = 0;

  void setTarget(RowSink* target);

protected:
  bool emitRow(SValue* row, int row_len);
  RowSink* target_;
};

}
}
#endif
