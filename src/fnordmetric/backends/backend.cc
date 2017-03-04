/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <fnordmetric/backends/backend.h>

namespace fnordmetric {

class NoopBackend : public Backend {
public:

  ReturnCode createTable(const TableConfig& table_config) override {
    return ReturnCode::success();
  }

  ReturnCode insertRows(const std::vector<InsertOp>& ops) override {
    for (const auto& op : ops) {
      std::cerr << "INSERT INTO " << op.table->table_id << std::endl;
      std::cerr << "  time = " << op.time << std::endl;
      for (const auto& c : op.columns) {
        std::cerr << "  " << c.first << " = " << c.second << std::endl;
      }
    }

    return ReturnCode::success();
  }

  void shutdown() override {}

};

ReturnCode Backend::openBackend(
    const URI& backend_uri,
    std::unique_ptr<Backend>* backend) {
  auto backend_type = backend_uri.scheme();

  if (backend_type == "noop") {
    backend->reset(new NoopBackend());
    return ReturnCode::success();
  }

  return ReturnCode::errorf("ERUNTIME", "invalid backend type: $0", backend_type);
}

} // namespace fnordmetric

