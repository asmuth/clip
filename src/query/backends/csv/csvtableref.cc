/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <memory>
#include <fnordmetric/query/backends/csv/csvtableref.h>
#include <fnordmetric/query/backends/csv/csvinputstream.h>
#include <fnordmetric/query/tableref.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {
namespace csv_backend {

CSVTableRef::CSVTableRef(
    std::unique_ptr<CSVInputStream>&& csv,
    bool headers /* = false */) :
    csv_(std::move(csv)),
    num_cols_(-1) {
  if (headers) {
    readHeaders();
  }
}

int CSVTableRef::getColumnIndex(const std::string& name) {
  const auto& header = headers_.find(name);

  if (header != headers_.end()) {
    return header->second;
  }

  return -1;
}

void CSVTableRef::executeScan(TableScan* scan) {
}

void CSVTableRef::readHeaders() {
  std::vector<std::string> headers;

  if (!csv_->readNextRow(&headers) || headers.size() == 0) {
    RAISE(util::RuntimeException, "no headers found in CSV file"); // FIXPAUL filename
  }

  size_t col_index = 0;
  for (const auto& header : headers) {
    headers_.emplace(header, col_index++);
  }

  num_cols_ = col_index;
}

}
}
}
