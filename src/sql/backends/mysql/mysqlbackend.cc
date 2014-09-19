/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/backends/mysql/mysqlbackend.h>
#include <fnordmetric/sql/backends/mysql/mysqlconnection.h>
#include <fnordmetric/sql/backends/mysql/mysqltableref.h>
#include <fnordmetric/util/runtimeexception.h>
#include <memory>
#include <mutex>
#include <mysql.h>

namespace fnordmetric {
namespace query {
namespace mysql_backend {

MySQLBackend* MySQLBackend::singleton() {
  static MySQLBackend singleton_backend;
  return &singleton_backend;
}

static std::mutex global_mysql_init_lock;
static bool global_mysql_initialized = false;

MySQLBackend::MySQLBackend() {
  global_mysql_init_lock.lock();
  if (!global_mysql_initialized) {
    mysql_library_init(0, NULL, NULL); // FIXPAUl mysql_library_end();
    global_mysql_initialized = true;
  }
  global_mysql_init_lock.unlock();
}

bool MySQLBackend::openTables(
    const std::vector<std::string>& table_names,
    const util::URI& source_uri,
    std::vector<std::unique_ptr<TableRef>>* target) {
  if (source_uri.scheme() != "mysql") {
    return false;
  }

  // FIXPAUL move all of this into a mysql thread/connection pool
  std::shared_ptr<MySQLConnection> conn =
      MySQLConnection::openConnection(source_uri);

  for (const auto& tbl : table_names) {
    target->emplace_back(new MySQLTableRef(conn, tbl));
  }

  connections_mutex_.lock();
  connections_.push_back(std::move(conn));
  connections_mutex_.unlock();

  return true;
}

}
}
}
