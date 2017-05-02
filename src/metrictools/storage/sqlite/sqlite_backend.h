/**
 * This file is part of the "FnordMetric" project
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
#include <metrictools/storage/backend.h>

namespace fnordmetric {
namespace sqlite_backend {

class SQLiteBackend : public Backend {
public:

  static ReturnCode connect(
      const URI& backend_uri,
      std::unique_ptr<Backend>* backend);

  ReturnCode performOperation(const InsertStorageOp& op) override;

protected:
  SQLiteBackend();

  std::mutex mutex_;
};

} // namespace sqlite_backend
} // namespace fnordmetric

