/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/backends/mysql/mysqlbackend.h>
#include <fnordmetric/backends/mysql/mysqlconnection.h>
#include <memory>
#include <mutex>

namespace fnordmetric {
namespace mysql_backend {

static std::mutex global_mysql_init_lock;
static bool global_mysql_initialized = false;

ReturnCode MySQLBackend::connect(
    const URI& backend_uri,
    std::unique_ptr<Backend>* backend) {
  global_mysql_init_lock.lock();
  if (!global_mysql_initialized) {
    mysql_library_init(0, NULL, NULL);
    global_mysql_initialized = true;
  }
  global_mysql_init_lock.unlock();

  std::unique_ptr<MySQLConnection> conn(new MySQLConnection());
  auto rc = conn->connect(backend_uri);
  if (!rc.isSuccess()) {
    return rc;
  }

  backend->reset(new MySQLBackend(std::move(conn)));
  return ReturnCode::success();
}

MySQLBackend::MySQLBackend(
    std::unique_ptr<MySQLConnection> conn) :
    conn_(std::move(conn)) {}

ReturnCode MySQLBackend::createTable(const TableConfig& table_config) {
  std::unique_lock<std::mutex> lk(mutex_);
  return ReturnCode::success();
}

ReturnCode MySQLBackend::insertRows(const std::vector<InsertOp>& ops) {
  std::unique_lock<std::mutex> lk(mutex_);
  return ReturnCode::success();
}

} // namespace mysql_backend
} // namespace fnordmetric

