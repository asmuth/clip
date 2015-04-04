/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/sql/backends/postgres/postgresbackend.h>
#include <fnordmetric/sql/backends/postgres/postgresconnection.h>
#include <fnordmetric/sql/backends/postgres/postgrestableref.h>
#include <fnordmetric/util/runtimeexception.h>
#include <memory>
#include <mutex>

namespace fnordmetric {
namespace query {
namespace postgres_backend {

PostgresBackend* PostgresBackend::singleton() {
  static PostgresBackend singleton_backend;
  return &singleton_backend;
}

static std::mutex global_postgres_init_lock;
static bool global_postgres_initialized = false;

PostgresBackend::PostgresBackend() {
#ifdef FNORD_ENABLE_POSTGRES
  global_postgres_init_lock.lock();
  if (!global_postgres_initialized) {
    global_postgres_initialized = true;
  }
  global_postgres_init_lock.unlock();
#endif
}

bool PostgresBackend::openTables(
    const std::vector<std::string>& table_names,
    const util::URI& source_uri,
    std::vector<std::unique_ptr<TableRef>>* target) {
  if (source_uri.scheme() != "postgres") {
    return false;
  }

  std::shared_ptr<PostgresConnection> conn =
      PostgresConnection::openConnection(source_uri);

  for (const auto& tbl : table_names) {
    target->emplace_back(new PostgresTableRef(conn, tbl));
  }

  connections_mutex_.lock();
  connections_.push_back(std::move(conn));
  connections_mutex_.unlock();

  return true;
}

}
}
}
