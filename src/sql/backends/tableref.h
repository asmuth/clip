/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_TABLEREF_H
#define _FNORDMETRIC_QUERY_TABLEREF_H
#include <stdlib.h>
#include <string>
#include <memory>

namespace fnordmetric {
namespace query {
class TableScan;

class TableRef {
public:
  virtual ~TableRef() {}
  virtual int getColumnIndex(const std::string& name) = 0;
  virtual void executeScan(TableScan* scan) = 0;
protected:
};

}
}
#endif
