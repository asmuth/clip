/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/storage/mysql/mysqlbackend.h>
#include <metrictools/storage/mysql/mysqlconnection.h>
#include <metrictools/util/logging.h>

namespace fnordmetric {
namespace mysql_backend {

MySQLConnection::MySQLConnection() : mysql_(nullptr) {}

MySQLConnection::~MySQLConnection() {
  if (mysql_) {
    mysql_close(mysql_);
  }
}

ReturnCode MySQLConnection::connect(const URI& uri) {
  mysql_ = mysql_init(NULL);

  if (mysql_ == nullptr) {
    return ReturnCode::error("ERUNTIME", "mysql_init() failed");
  }

  unsigned int port = 3306;
  std::string host = uri.host();
  std::string username;
  std::string password;
  std::string database;

  if (host.size() == 0) {
    return ReturnCode::errorf(
        "ERUNTIME",
        "invalid mysql:// URI: has no hostname (URI: '$0')",
        uri.toString());
  }

  if (uri.port() > 0) {
    port = uri.port();
  }

  if (uri.path().size() < 2 || uri.path()[0] != '/') {
    return ReturnCode::errorf(
        "ERUNTIME",
        "invalid mysql:// URI: missing database, format is: mysql://host/db "
        " (URI: $0)",
        uri.toString());
  }

  database = uri.path().substr(1);

  for (const auto& param : uri.queryParams()) {
    if (param.first == "username" || param.first == "user") {
      username = param.second;
      continue;
    }

    if (param.first == "password" || param.first == "pass") {
      password = param.second;
      continue;
    }

    return ReturnCode::errorf(
        "ERUNTIME",
        "invalid parameter for mysql:// URI: '$0=$1'",
        param.first,
        param.second);
  }

  return connect(host, port, database, username, password);
}

ReturnCode MySQLConnection::connect(
    const std::string& host,
    unsigned int port,
    const std::string& database,
    const std::string& username,
    const std::string& password) {
  auto ret = mysql_real_connect(
      mysql_,
      host.c_str(),
      username.size() > 0 ? username.c_str() : NULL,
      password.size() > 0 ? password.c_str() : NULL,
      database.size() > 0 ? database.c_str() : NULL,
      port,
      NULL,
      CLIENT_COMPRESS);

  if (ret != mysql_) {
    return ReturnCode::errorf(
        "ERUNTIME",
        "mysql_real_connect() failed: $0",
        mysql_error(mysql_));
  }

  return ReturnCode::success();
}

ReturnCode MySQLConnection::describeTable(
    const std::string& table_name,
    std::vector<std::string>* columns) {
  MYSQL_RES* res = mysql_list_fields(mysql_, table_name.c_str(), NULL);
  if (res == nullptr) {
    return ReturnCode::errorf(
        "ERUNTIME",
        "mysql_list_fields() failed: $0",
        mysql_error(mysql_));
  }

  auto num_cols = mysql_num_fields(res);
  for (int i = 0; i < num_cols; ++i) {
    MYSQL_FIELD* col = mysql_fetch_field_direct(res, i);
    columns->emplace_back(col->name);
  }

  mysql_free_result(res);
  return ReturnCode::success();
}

ReturnCode MySQLConnection::executeQuery(
    const std::string& query,
    std::vector<std::string>* header /* = nullptr */,
    std::list<std::vector<std::string>>* rows /* = nullptr */) {
  logDebug("[MySQL] Executing Query: $0", query);

  if (mysql_real_query(mysql_, query.c_str(), query.size()) != 0) {
    return ReturnCode::errorf(
        "ERUNTIME",
        "mysql query failed: $0",
        mysql_error(mysql_));
  }

  if (rows) {
    auto result = mysql_use_result(mysql_);
    if (!result) {
      return ReturnCode::success();
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
      auto col_lens = mysql_fetch_lengths(result);
      if (col_lens == nullptr) {
        break;
      }

      std::vector<std::string> row_vec;
      auto row_len = mysql_num_fields(result);
      for (int i = 0; i < row_len; ++i) {
        row_vec.emplace_back(row[i], col_lens[i]);
      }

      rows->emplace_back(row_vec);
    }

    mysql_free_result(result);
  }

  return ReturnCode::success();
}

std::string MySQLConnection::escapeString(const std::string& in) {
  std::string out;
  out.resize(in.size() * 2 + 1);
  auto len = mysql_real_escape_string(mysql_, &out[0], in.data(), in.size());
  out.resize(len);
  return out;
}

} // namespace mysql_backend
} // namespace fnordmetric

