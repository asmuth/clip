/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CSVTABLEREF_H
#define _FNORDMETRIC_CSVTABLEREF_H
#include <memory>
#include <unordered_map>
#include <string>
#include <fnordmetric/query/backends/csv/csvinputstream.h>
#include <fnordmetric/query/tableref.h>

namespace fnordmetric {
namespace query {
class SValue;
namespace csv_backend {

/**
 * A CSVTableRef instance is threadfriendly but not threadsafe. You must
 * synchronize access to all methods.
 */
class CSVTableRef : public TableRef {
public:
  CSVTableRef(
      std::unique_ptr<CSVInputStream>&& csv,
      bool headers = false);

  int getColumnIndex(const std::string& name) override;
  void executeScan(TableScan* scan) override;

  bool readNextRow(std::vector<SValue>* target);
  void readHeaders();

protected:
  std::unordered_map<std::string, size_t> headers_;
  std::unique_ptr<CSVInputStream> csv_;
  int num_cols_;
  int min_cols_;
  int row_index_;
};

}
}
}
#endif
