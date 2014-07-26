/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <memory>
#include <query/tableref.h>
#include <query/backends/csv/csvtableref.h>

namespace fnordmetric {
namespace query {
namespace csv_backend {

class CSVTableRef : public TableRef {
public:
  CSVTableRef(std::unique_ptr<CSVInputStream>&& csv);
  int getColumnIndex(const std::string& name) override;
  void executeScan(TableScan* scan) override;
protected:
  std::unique_ptr<CSVInputStream> csv_;
};

}
}
}
#endif
