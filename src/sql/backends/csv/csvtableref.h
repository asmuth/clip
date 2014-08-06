/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_CSVTABLEREF_H
#define _FNORDMETRIC_CSVTABLEREF_H
#include <memory>
#include <unordered_map>
#include <string>
#include <fnordmetric/sql/backends/csv/csvinputstream.h>
#include <fnordmetric/sql/tableref.h>

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
