/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_TABLEREF_H
#define _FNORDMETRIC_QUERY_TABLEREF_H
#include <stdlib.h>
#include <string>
#include <memory>

namespace fnordmetric {
namespace query {

class TableRef {
public:
  virtual ~TableRef() {}
  virtual int getColumnIndex(const std::string& name) = 0;
protected:
};

}
}
#endif
