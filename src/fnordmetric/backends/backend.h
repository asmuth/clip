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
#include <fnordmetric/util/return_code.h>
#include <fnordmetric/table.h>
#include <libtransport/uri/uri.h>

namespace fnordmetric {

class Backend {
public:

  static ReturnCode openBackend(
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

  virtual ReturnCode executeQuery(
      const std::string& query,
      std::vector<std::string>* header = nullptr,
      std::list<std::vector<std::string>>* rows = nullptr) = 0;

  virtual void shutdown() {}

};

} // namespace fnordmetric

