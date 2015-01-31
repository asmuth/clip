/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_SQL_ROWSINK_H
#define _FNORDMETRIC_SQL_ROWSINK_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include <sql/svalue.h>
#include <sql/parser/token.h>
#include <sql/parser/astnode.h>

namespace fnordmetric {
namespace query {

class RowSink {
public:
  virtual ~RowSink() {}
  virtual bool nextRow(SValue* row, int row_len) = 0;
  virtual void finish() {}
};

}
}
#endif
