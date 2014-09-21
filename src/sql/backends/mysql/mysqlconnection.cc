/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/sql/backends/mysql/mysqlconnection.h>

namespace fnordmetric {
namespace query {
namespace mysql_backend {

std::unique_ptr<MySQLConnection> MySQLConnection::openConnection(
    const util::URI& uri) {
  std::unique_ptr<MySQLConnection> conn(new MySQLConnection());
  conn->connect(uri);
  return conn;
}

MySQLConnection::MySQLConnection() : mysql_(nullptr) {
  mysql_ = mysql_init(NULL);

  if (mysql_ == nullptr) {
    RAISE(util::RuntimeException, "mysql_init() failed\n");
  }
}

MySQLConnection::~MySQLConnection() {
  mysql_close(mysql_);
}

void MySQLConnection::connect(const util::URI& uri) {
  unsigned int port = 3306;
  std::string host = uri.host();
  std::string username;
  std::string password;
  std::string database;

  if (host.size() == 0) {
    RAISE(
        util::RuntimeException,
        "invalid mysql:// URI: has no hostname (URI: '%s')",
        uri.toString().c_str());
  }

  if (uri.port() > 0) {
    port = uri.port();
  }

  if (uri.path().size() < 2 || uri.path()[0] != '/') {
    RAISE(
        util::RuntimeException,
        "invalid mysql:// URI: missing database, format is: mysql://host/db "
        " (URI: %s)",
        uri.toString().c_str());
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

    RAISE(
        util::RuntimeException,
        "invalid parameter for mysql:// URI: '%s=%s'",
        param.first.c_str(),
        param.second.c_str());
  }

  connect(host, port, database, username, password);
}

void MySQLConnection::connect(
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
    RAISE(
      util::RuntimeException,
      "mysql_real_connect() failed: %s\n",
      mysql_error(mysql_));
  }
}


std::vector<std::string> MySQLConnection::describeTable(
    const std::string& table_name) {
  MYSQL_RES* res = mysql_list_fields(mysql_, table_name.c_str(), NULL);
  if (res == nullptr) {
    RAISE(
      util::RuntimeException,
      "mysql_list_fields() failed: %s\n",
      mysql_error(mysql_));
  }

  std::vector<std::string> columns;
  auto num_cols = mysql_num_fields(res);
  for (int i = 0; i < num_cols; ++i) {
    MYSQL_FIELD* col = mysql_fetch_field_direct(res, i);
    columns.emplace_back(col->name);
  }

  mysql_free_result(res);
  return columns;
}

void MySQLConnection::executeQuery(
    const std::string& query,
    std::function<bool (const std::vector<std::string>&)> row_callback) {
  if (env()->verbose()) {
    util::LogEntry entry;
    entry.append("__severity__", "DEBUG");
    entry.append("__message__", "Executing MySQL query");
    entry.append("query", query);
    env()->logger()->log(entry);
  }

  MYSQL_RES* result = nullptr;
  if (mysql_real_query(mysql_, query.c_str(), query.size()) == 0) {
    result = mysql_use_result(mysql_);
  }

  if (result == nullptr) {
    RAISE(
        util::RuntimeException,
        "mysql query failed: %s -- error: %s\n",
        query.c_str(),
        mysql_error(mysql_));
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

    try {
      if (!row_callback(row_vec)) {
        break;
      }
    } catch (const std::exception& e) {
      mysql_free_result(result);
      try {
        auto rte = dynamic_cast<const util::RuntimeException&>(e);
        throw rte;
      } catch (std::bad_cast bce) {
        throw e;
      }
    }
  }

  mysql_free_result(result);
}

}
}
}
