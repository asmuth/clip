/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/backends/mysql/mysqltableref.h>
#include <fnordmetric/sql/tablescan.h>

namespace fnordmetric {
namespace query {
namespace mysql_backend {

MySQLTableRef::MySQLTableRef(
      std::shared_ptr<MySQLConnection> conn,
      const std::string& table_name) :
      conn_(conn),
      table_name_(table_name) {
  columns_ = conn->describeTable(table_name_);
}

int MySQLTableRef::getColumnIndex(const std::string& name) {
  for (int i = 0; i < columns_.size(); ++i) {
    // FIXPAUL case insensitive match
    if (columns_[i] == name) {
      return i;
    }
  }

  return -1;
}

void MySQLTableRef::executeScan(TableScan* scan) {
  std::string mysql_query = "SELECT";

  auto columns = scan->getColumns();
  for (int i = 0; i < columns.size(); ++i) {
    mysql_query.append(i == 0 ? " " : ",");
    mysql_query.append("`");
    mysql_query.append(columns[i]); // FIXPAUL escape?
    mysql_query.append("`");
  }

  mysql_query.append(" FROM ");
  mysql_query.append(table_name_);

  conn_->executeQuery(
      mysql_query,
      [] (const std::vector<std::string>& row) -> bool {
        printf("row!\n");
        return true;
      });
}

}
}
}
