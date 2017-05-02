/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <libtransport/uri/uri.h>
#include <metrictools/util/return_code.h>
#include <mysql/mysql.h>

namespace fnordmetric {
namespace mysql_backend {

class MySQLConnection {
public:

  /**
   * Creates a new mysql connection and tries to connect to the provided URI
   * May throw an exception
   *
   * Example URIs:
   *    mysql://localhost/test_database?user=root
   *    mysql://somehost:3306/my_database?user=my_user&password=my_secret
   *
   * @param URI the mysql:// URI
   * @returns a new MySQLConnection
   */
  static std::unique_ptr<MySQLConnection> openConnection(const URI& uri);

  /**
   * Create a new mysql connection
   */
  MySQLConnection();

  /**
   * Destroy a mysql connection. Closes the connection
   */
  ~MySQLConnection();

  /**
   * Connect to a mysql server on the specified URI.
   *
   * Example URIs:
   *    mysql://localhost/test_database?user=root
   *    mysql://somehost:3306/my_database?user=my_user&password=my_secret
   *
   * @param URI the mysql:// URI
   * @returns a new MySQLConnection
   */
  ReturnCode connect(const URI& uri);

  /**
   * Connect to a mysql server.
   *
   * @param host the mysql server hostname
   * @param port the mysql server port
   * @param database the mysql database to open or empty string
   * @param username the mysql user username or empty string
   * @param password the mysq user password or empty string
   * @returns a new MySQLConnection
   */
  ReturnCode connect(
      const std::string& host,
      unsigned int port,
      const std::string& database,
      const std::string& username,
      const std::string& password);

  /**
   * Returns a list of all column names for the provided table name. May 
   * throw an exception (This does the equivalent to a DESCRIBE TABLE)
   *
   * @param table_name the name of the table do describe
   * @returns a list of all columns names of the table
   */
  ReturnCode describeTable(
      const std::string& table_name,
      std::vector<std::string>* columns);

  /**
   * Execute a mysql query. The mysql query string must not include a terminal
   * semicolon.
   *
   * The provided row callback will be called for every row in the result set.
   * The row callback must return a boolean value; if it returns true it will
   * be called again for the next row in the result set (if a next row exists),
   * if it returns false it will not be called again and the remainder of the
   * result set will be discarded.
   *
   * @param query the mysql query string without a terminal semicolon
   */
  ReturnCode executeQuery(
      const std::string& query,
      std::vector<std::string>* header = nullptr,
      std::list<std::vector<std::string>>* rows = nullptr);

  std::string escapeString(const std::string& str);

protected:
   MYSQL* mysql_;
};

} // namespace mysql_backend
} // namespace fnordmetric

