/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/storage/sqlite/sqlite_backend.h>

namespace fnordmetric {
namespace sqlite_backend {

ReturnCode SQLiteBackend::connect(
    const URI& backend_uri,
    std::unique_ptr<Backend>* backend) {
  backend->reset(new SQLiteBackend());
  return ((SQLiteBackend*) backend->get())->open(backend_uri.path());
}

SQLiteBackend::SQLiteBackend() : db_(nullptr) {}

SQLiteBackend::~SQLiteBackend() {
  if (db_) {
    sqlite3_close(db_);
  }
}

ReturnCode SQLiteBackend::performOperation(const InsertStorageOp& op) {
  std::cerr << "PERFORM: " << op;
  return executeQuery("select 1;");
}

ReturnCode SQLiteBackend::open(const std::string& path) {
  if (sqlite3_open(path.c_str(), &db_)){
    return ReturnCode::errorf("EIO", "sqlite error: $0", sqlite3_errmsg(db_));
  }

  return ReturnCode::success();
}

ReturnCode SQLiteBackend::executeQuery(
    const std::string& query,
    std::list<std::vector<std::string>>* rows /* = nullptr */,
    std::vector<std::string>* columns /* = nullptr */) {
  char* err = nullptr;
  auto rc = sqlite3_exec(db_, query.c_str(), nullptr, 0, &err);
  if (rc != SQLITE_OK){
    auto err_str = std::string(err);
    sqlite3_free(err);
    return ReturnCode::errorf("EIO", "sqlite error: $0", err_str);
  }

  return ReturnCode::success();
}

} // namespace sqlite_backend
} // namespace fnordmetric

