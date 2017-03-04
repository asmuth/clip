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
#include <vector>
#include <iostream>
#include <libtransport/uri/uri.h>

namespace fnordmetric {

class Backend {
public:

  ReturnCode openBackend(
      const URI& backend_uri,
      std::unique_ptr<Backend>* backend);

  virtual ~Backend() = default;

  virtual ReturnCode createTable(const TableConfig& table_config) = 0;

  struct InsertOp {
    std::shared_ptr<TableConfig> table;
    uint64_t time;
    std::vector<std::pair<std::string, std::string>> columns;
  };

  virtual ReturnCode insertRows(const std::vector<InsertOp>& ops) = 0;

  virtual void shutdown() = 0;

};

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

} // namespace fnordmetric

