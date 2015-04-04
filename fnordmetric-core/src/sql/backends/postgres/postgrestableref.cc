/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/backends/postgres/postgrestableref.h>
#include <fnordmetric/sql/runtime/tablescan.h>
#include <iostream>

namespace fnordmetric {
namespace query {
namespace postgres_backend {

PostgresTableRef::PostgresTableRef(
      std::shared_ptr<PostgresConnection> conn,
      const std::string& table_name) :
      conn_(conn),
      table_name_(table_name) {
  table_columns_ = conn->describeTable(table_name_); // FIXPAUL cache me
}

std::vector<std::string> PostgresTableRef::columns() {
  std::vector<std::string> cols;

  for (const auto& col : columns_) {
    cols.emplace_back(*col);
  }

  return cols;
}

int PostgresTableRef::getColumnIndex(const std::string& name) {
  for (int i = 0; i < columns_.size(); ++i) {
    // FIXPAUL case insensitive match
    if (*columns_[i] == name) {
      return i;
    }
  }

  for (int i = 0; i < table_columns_.size(); ++i) {
    // FIXPAUL case insensitive match
    if (table_columns_[i] == name) {
      columns_.emplace_back(&table_columns_[i]);
      return columns_.size() - 1;
    }
  }

  return -1;
}

std::string PostgresTableRef::getColumnName(int index) {
  if (index >= columns_.size()) {
    RAISE(kIndexError, "no such column");
  }

  return *columns_[index];
}

void PostgresTableRef::executeScan(TableScan* scan) {
  std::string postgres_query = "SELECT";

  for (int i = 0; i < columns_.size(); ++i) {
    postgres_query.append(i == 0 ? " " : ",");
    postgres_query.append("\"");
    postgres_query.append(*columns_[i]); // FIXPAUL escape?
    postgres_query.append("\"");
  }

  postgres_query.append(" FROM ");
  postgres_query.append(table_name_);

  conn_->executeQuery(
      postgres_query,
      [this, scan] (const std::vector<std::string>& row) -> bool {
        std::vector<SValue> row_svals;

        for (const auto& col : row) {
          row_svals.emplace_back(col);
        }

        return scan->nextRow(row_svals.data(), row_svals.size());
      });
}

}
}
}
