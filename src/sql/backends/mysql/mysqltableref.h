/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_MYSQLBACKEND_MYSQLTABLEREF_H
#define _FNORDMETRIC_MYSQLBACKEND_MYSQLTABLEREF_H
#include <memory>
#include <fnordmetric/sql/backends/mysql/mysqlconnection.h>
#include <fnordmetric/sql/tableref.h>

namespace fnordmetric {
namespace query {
namespace mysql_backend {

class MySQLTableRef : public TableRef {
public:

  MySQLTableRef(
      std::shared_ptr<MySQLConnection> conn,
      const std::string& table_name);

  int getColumnIndex(const std::string& name) override;
  void executeScan(TableScan* scan) override;

protected:
  std::shared_ptr<MySQLConnection> conn_;
  std::vector<std::string> columns_;
};

}
}
}
#endif
