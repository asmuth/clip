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
#include <fnordmetric/sql/backends/postgres/postgresconnection.h>
#include <iostream>
#include <sstream>

namespace fnordmetric {
namespace query {
namespace postgres_backend {

std::unique_ptr<PostgresConnection> PostgresConnection::openConnection(
    const util::URI& uri) {
  std::unique_ptr<PostgresConnection> conn(new PostgresConnection());
  conn->connect(uri);
  return conn;
}

PostgresConnection::PostgresConnection() : postgres_(nullptr) {
#ifdef FNORD_ENABLE_POSTGRES
  //postgres_ = PQconnectdb(NULL);

  //if (postgres_ == nullptr) {
    //RAISE(kRuntimeError, "postgres_init() failed\n");
  //}
#else
  RAISE(kRuntimeError, "FnordMetric was compiled without libpostgresclient");
#endif
}

PostgresConnection::~PostgresConnection() {
#ifdef FNORD_ENABLE_POSTGRES
  if (postgres_)
    PQfinish(postgres_);
#else
  RAISE(kRuntimeError, "FnordMetric was compiled without libpostgresclient");
#endif
}

void PostgresConnection::connect(const util::URI& uri) {
  unsigned int port = 5432;
  std::string host = uri.host();
  std::string username;
  std::string password;
  std::string database;

  if (host.size() == 0) {
    RAISE(
        kRuntimeError,
        "invalid postgres:// URI: has no hostname (URI: '%s')",
        uri.toString().c_str());
  }

  if (uri.port() > 0) {
    port = uri.port();
  }

  if (uri.path().size() < 2 || uri.path()[0] != '/') {
    RAISE(
        kRuntimeError,
        "invalid postgres:// URI: missing database, format is: postgres://host/db "
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
        kRuntimeError,
        "invalid parameter for postgres:// URI: '%s=%s'",
        param.first.c_str(),
        param.second.c_str());
  }

  connect(host, port, database, username, password);
}

void PostgresConnection::connect(
    const std::string& host,
    unsigned int port,
    const std::string& database,
    const std::string& username,
    const std::string& password) {
#ifdef FNORD_ENABLE_POSTGRES
  std::stringstream conn;
  if (username.length())
    conn << "user=" << username << " ";
  if (password.length())
  conn << "password=" << password << " ";
  if (database.length())
  conn << "dbname=" << database << " ";

  conn << "hostaddr=" << host << " ";
  conn << "port=" << port;

  postgres_ = PQconnectdb(conn.str().c_str());
  if (PQstatus(postgres_) != CONNECTION_OK) {
    RAISE(
      kRuntimeError,
      "PQconnectdb() failed: %s\n",
      PQerrorMessage(postgres_));
  }
#else
  RAISE(kRuntimeError, "FnordMetric was compiled without libpostgresclient");
#endif
}


std::vector<std::string> PostgresConnection::describeTable(
    const std::string& table_name) {
  std::vector<std::string> columns;

#ifdef FNORD_ENABLE_POSTGRES
  // construct attribute names query
  std::stringstream query;
  query << "select attname from pg_attribute join pg_class on attrelid = pg_class.oid where relname = '";
  query << table_name;
  query << "' and attnum > 0";
  
  // run query
  PGresult* res = PQexec(postgres_, query.str().c_str());
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    abort();
    RAISE(
      kRuntimeError,
      "PQexec() failed: %s\n",
      PQerrorMessage(postgres_));
  }

  // retrieve result and add to columns vector
  auto num_rows = PQntuples(res);
  for (int i = 0; i < num_rows; ++i) {
    columns.emplace_back(PQgetvalue(res,i,0));
  }

  PQclear(res);
#else
  RAISE(kRuntimeError, "FnordMetric was compiled without libpostgresclient");
#endif
  return columns;
}

void PostgresConnection::executeQuery(
    const std::string& query,
    std::function<bool (const std::vector<std::string>&)> row_callback) {
#ifdef FNORD_ENABLE_POSTGRES
  if (env()->verbose()) {
    fnord::util::LogEntry entry;
    entry.append("__severity__", "DEBUG");
    entry.append("__message__", "Executing Postgres query");
    entry.append("query", query);
    env()->logger()->log(entry);
  }

  // run query
  PGresult* res = PQexec(postgres_, query.c_str());
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    RAISE(
      kRuntimeError,
      "PQexec() failed: %s\n",
      PQerrorMessage(postgres_));
  }

  // retrieve result and add to columns vector
  auto num_rows = PQntuples(res);
  for (int r = 0; r < num_rows; ++r) {
    auto num_cols = PQnfields(res);
    std::vector<std::string> row_vec;
    for (int c = 0; c < num_cols; ++c) {
      row_vec.emplace_back(PQgetvalue(res,r,c));
    }
    
    try {
      if (!row_callback(row_vec)) {
        break;
      }
    } catch (const std::exception& e) {
      PQclear(res);
      try {
        auto rte = dynamic_cast<const util::RuntimeException&>(e);
        throw rte;
      } catch (std::bad_cast bce) {
        throw e;
      }
    }
  }

  PQclear(res);
#else
  RAISE(kRuntimeError, "FnordMetric was compiled without libpostgresclient");
#endif
}

}
}
}
