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

namespace fnordmetric {
namespace query {
namespace mysql_backend {

class MySQLTableRef {
public:

  MySQLTableRef(
      std::shared_ptr<MySQLConnection> conn,
      const std::string& table_name);

protected:
  std::shared_ptr<MySQLConnection> conn_;
};

}
}
}
#endif
