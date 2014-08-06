/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <memory>
#include <fnordmetric/sql/backends/csv/csvtableref.h>
#include <fnordmetric/sql/backends/csv/csvinputstream.h>
#include <fnordmetric/sql/svalue.h>
#include <fnordmetric/sql/tableref.h>
#include <fnordmetric/sql/tablescan.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {
namespace csv_backend {

CSVTableRef::CSVTableRef(
    std::unique_ptr<CSVInputStream>&& csv,
    bool headers /* = false */) :
    csv_(std::move(csv)),
    num_cols_(-1),
    min_cols_(0),
    row_index_(0) {
  if (headers) {
    readHeaders();
  }
}

int CSVTableRef::getColumnIndex(const std::string& name) {
  const auto& header = headers_.find(name);

  if (header != headers_.end()) {
    return header->second;
  }

  if (name.size() > 3 &&
      strncmp(name.c_str(), "col", 3) == 0) {
    int index = -1;

    try {
      index = std::stoi(std::string(name.c_str() + 3, name.size() - 3));
    } catch (std::exception e) {
      return -1;
    }

    if (index > 0) {
      return index - 1;
    }
  }

  return -1;
}

void CSVTableRef::executeScan(TableScan* scan) {
  for (;;) {
    std::vector<SValue> row;

    if (!readNextRow(&row)) {
      return;
    }

    if (!scan->nextRow(row.data(), row.size())) {
      return;
    }
  }
}

bool CSVTableRef::readNextRow(std::vector<SValue>* target) {
  std::vector<std::string> row;

  if (!csv_->readNextRow(&row) || row.size() == 0) {
    return false;
  } else {
    row_index_++;
  }

  if (row.size() < min_cols_) {
    RAISE(
        util::RuntimeException,
        "csv row #%i does not have enough columns -- "
        "columns found=%i, required=%i\n",
        row_index_,
        row.size(),
        min_cols_); // FIXPAUL filename
  }

  if (num_cols_ == -1) {
    num_cols_ = row.size();
  } else if (row.size() != num_cols_) {
    RAISE(
        util::RuntimeException,
        "csv row #%i does not have the same number of columns as the previous "
        "line -- number of columns found=%i, previous=%i\n",
        row_index_,
        row.size(),
        num_cols_); // FIXPAUL filename
  }

  for (const auto& col : row) {
    target->emplace_back(col);
  }

  return true;
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
  min_cols_ = col_index;
  row_index_++;
}

}
}
}
