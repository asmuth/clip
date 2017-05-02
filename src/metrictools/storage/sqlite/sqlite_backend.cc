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
  return ReturnCode::success();
}

SQLiteBackend::SQLiteBackend() {}

ReturnCode SQLiteBackend::performOperation(const InsertStorageOp& op) {
  std::cerr << "PERFORM: " << op;
  return ReturnCode::success();
}

} // namespace sqlite_backend
} // namespace fnordmetric

