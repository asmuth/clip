/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_POSTGRESBACKEND_POSTGRESCONNECTION_H
#define _FNORDMETRIC_POSTGRESBACKEND_POSTGRESCONNECTION_H
#include <memory>
#include <fnordmetric/util/uri.h>
#include <fnordmetric/util/runtimeexception.h>
#include <functional>
#ifdef FNORD_ENABLE_POSTGRES
#include <libpq-fe.h>
#endif

namespace fnordmetric {
namespace query {
namespace postgres_backend {

class PostgresConnection {
public:

  /**
   * Creates a new postgres connection and tries to connect to the provided URI
   * May throw an exception
   *
   * Example URIs:
   *    postgres://localhost/test_database?user=root
   *    postgres://somehost:3306/my_database?user=my_user&password=my_secret
   *
   * @param URI the postgres:// URI
   * @returns a new PostgresConnection
   */
  static std::unique_ptr<PostgresConnection> openConnection(const util::URI& uri);

  /**
   * Create a new postgres connection
   */
  PostgresConnection();

  /**
   * Destroy a postgres connection. Closes the connection
   */
  ~PostgresConnection();

  /**
   * Connect to a postgres server on the specified URI. May throw an exception
   *
   * Example URIs:
   *    postgres://localhost/test_database?user=root
   *    postgres://somehost:3306/my_database?user=my_user&password=my_secret
   *
   * @param URI the postgres:// URI
   * @returns a new PostgresConnection
   */
  void connect(const util::URI& uri);

  /**
   * Connect to a postgres server. May throw an exception
   *
   * @param host the postgres server hostname
   * @param port the postgres server port
   * @param database the postgres database to open or empty string
   * @param username the postgres user username or empty string
   * @param password the mysq user password or empty string
   * @returns a new PostgresConnection
   */
  void connect(
      const std::string& host,
      unsigned int port,
      const std::string& database,
      const std::string& username,
      const std::string& password);

  /**
   * Returns a list of all column names for the provided table name. May 
   * throw an exception (This does the equivalent to a DESCRIBEL TABLE)
   *
   * @param table_name the name of the table do describe
   * @returns a list of all columns names of the table
   */
  std::vector<std::string> describeTable(const std::string& table_name);

  /**
   * Execute a postgres query. The postgres query string must not include a terminal
   * semicolon.
   *
   * The provided row callback will be called for every row in the result set.
   * The row callback must return a boolean value; if it returns true it will
   * be called again for the next row in the result set (if a next row exists),
   * if it returns false it will not be called again and the remainder of the
   * result set will be discarded.
   *
   * This method may throw an exception.
   *
   * @param query the postgres query string without a terminal semicolon
   * @param row_callback the callback that should be called for every result row
   */
  void executeQuery(
      const std::string& query,
      std::function<bool (const std::vector<std::string>&)> row_callback);

protected:
#ifdef FNORD_ENABLE_POSTGRES
   PGconn* postgres_;
#else
   void* postgres_;
#endif
};

}
}
}
#endif
