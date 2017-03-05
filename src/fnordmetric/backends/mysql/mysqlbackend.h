/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <fnordmetric/backends/backend.h>
#include <fnordmetric/backends/mysql/mysqlconnection.h>

namespace fnordmetric {
namespace mysql_backend {

class MySQLBackend : public Backend {
public:

  static ReturnCode connect(
      const URI& backend_uri,
      std::unique_ptr<Backend>* backend);

  ReturnCode createTable(const TableConfig& table_config) override;
  ReturnCode insertRows(const std::vector<InsertOp>& ops) override;

  ReturnCode executeQuery(
      const std::string& query,
      std::vector<std::string>* header = nullptr,
      std::list<std::vector<std::string>>* rows = nullptr) override;

protected:

  MySQLBackend(std::unique_ptr<MySQLConnection> conn);

  std::unique_ptr<MySQLConnection> conn_;
  std::mutex mutex_;
};

} // namespace mysql_backend
} // namespace fnordmetric

