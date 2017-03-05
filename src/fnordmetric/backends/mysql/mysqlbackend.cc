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
#include <fnordmetric/util/time.h>
#include <fnordmetric/util/logging.h>
#include <fnordmetric/aggregate.h>
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

static std::string getMySQLType(DataType t) {
  switch (t) {
    case DataType::UINT64:
    case DataType::INT64:
      return "BIGINT";
    case DataType::FLOAT64:
      return "DOUBLE";
    case DataType::NIL:
    case DataType::STRING:
      return "VARCHAR(1024)"; // FIXME
    case DataType::TIMESTAMP:
      return "TIMESTAMP";
  }

  throw std::runtime_error("invalid data type");
}

ReturnCode MySQLBackend::createTable(const TableConfig& table_config) {
  std::vector<std::pair<std::string, DataType>> expected_cols;
  expected_cols.emplace_back("time", DataType::TIMESTAMP);
  expected_cols.emplace_back("time_window", DataType::FLOAT64);

  for (const auto& l : table_config.labels) {
    expected_cols.emplace_back(l.column_name, l.type);
  }

  for (const auto& m : table_config.measures) {
    auto rc = getAggregationFunctionOutputColumns(m, &expected_cols);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  std::unique_lock<std::mutex> lk(mutex_);

  /* check if table exists */
  bool table_exists = false;
  {
    auto query = StringUtil::format(
        "SHOW TABLES LIKE '$0';",
        conn_->escapeString(table_config.table_id));

    std::vector<std::string> headers;
    std::list<std::vector<std::string>> rows;
    auto rc = conn_->executeQuery(query, &headers, &rows);
    if (!rc.isSuccess()) {
      return rc;
    }

    if (rows.size() > 0) {
      table_exists = true;
    }
  }

  if (table_exists) {
    /* migrate */
    std::set<std::string> actual_columns;
    {
      std::vector<std::string> cols;
      auto rc = conn_->describeTable(table_config.table_id, &cols);
      if (!rc.isSuccess()) {
        return rc;
      }

      for (const auto& c : cols) {
        actual_columns.insert(c);
      }
    }

    for (const auto& c : expected_cols) {
      if (actual_columns.count(c.first) > 0) {
        continue;
      }

      logInfo(
          "[MySQL] Creating table column: $0.$1",
          table_config.table_id,
          c.first);

      std::string query = StringUtil::format(
          "ALTER TABLE `$0` ADD COLUMN `$1` $2;",
          conn_->escapeString(table_config.table_id),
          conn_->escapeString(c.first),
          getMySQLType(c.second));

      auto rc = conn_->executeQuery(query);
      if (!rc.isSuccess()) {
        return rc;
      }
    }

    return ReturnCode::success();
  } else {
    /* create */
    logInfo("[MySQL] Creating table: $0", table_config.table_id);

    std::vector<std::string> create_cols;
    for (const auto& c : expected_cols) {
      create_cols.emplace_back(
          StringUtil::format(
              "`$0` $1",
              conn_->escapeString(c.first),
              getMySQLType(c.second)));
    }

    std::string query = StringUtil::format(
        "CREATE TABLE `$0` ($1);",
        conn_->escapeString(table_config.table_id),
        StringUtil::join(create_cols, ", "));

    return conn_->executeQuery(query);
  }
}

ReturnCode MySQLBackend::insertRows(const std::vector<InsertOp>& ops) {
  std::unique_lock<std::mutex> lk(mutex_);

  for (const auto& op : ops) {
    std::vector<std::string> insert_cols;

    insert_cols.emplace_back(
        StringUtil::format(
            "time='$0'",
            conn_->escapeString(UnixTime(op.time).toString())));

    insert_cols.emplace_back(
        StringUtil::format(
            "time_window='$0'",
            conn_->escapeString(
                std::to_string(op.table->interval / double(kMicrosPerSecond)))));

    for (const auto& c : op.columns) {
      insert_cols.emplace_back(
          StringUtil::format(
              "`$0`='$1'",
              conn_->escapeString(c.first),
              conn_->escapeString(c.second)));
    }

    std::string query = StringUtil::format(
        "INSERT INTO `$0` SET $1;",
        conn_->escapeString(op.table->table_id),
        StringUtil::join(insert_cols, ", "));

    auto rc = conn_->executeQuery(query);
    if (!rc.isSuccess()) {
      return ReturnCode::error("[MySQL] Insert failed: $0", rc.getMessage());
    }
  }

  return ReturnCode::success();
}

} // namespace mysql_backend
} // namespace fnordmetric

