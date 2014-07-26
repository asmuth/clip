/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CSVTABLEREF_H
#define _FNORDMETRIC_CSVTABLEREF_H
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace csv_backend {

class CSVTableRef : public fnordmetric::query::TableRef {
public:

  int getColumnIndex(const std::string& name) override;
  void executeScan(TableScan* scan) override;
};

}
}
#endif;
