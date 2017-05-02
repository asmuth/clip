/**
 * This file is part of the "FnordTable" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordTable is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <metrictools/table.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace fnordmetric {

class TableMap {
friend class TableMapBuilder;
public:

  ~TableMap();

  std::shared_ptr<TableConfig> findTable(const std::string& key) const;

  std::set<std::string> listTables() const;

protected:

  using SlotType = std::shared_ptr<TableConfig>;
  using MapType = std::map<std::string, SlotType>;
  using IterType = MapType::iterator;

  MapType tables_;
};

class TableMapBuilder {
public:

  TableMapBuilder();

  void addTable(
      const TableIDType& table_id,
      const TableConfig& table);

  std::shared_ptr<TableConfig> findTable(const TableIDType& table_id);

  std::shared_ptr<TableMap> getTableMap();

protected:
  std::shared_ptr<TableMap> table_map_;
};

class VersionedTableMap {
public:

  VersionedTableMap();

  std::shared_ptr<TableMap> getTableMap() const;

  void updateTableMap(std::shared_ptr<TableMap> table_map);

protected:
  mutable std::mutex mutex_;
  std::shared_ptr<TableMap> table_map_;
};

} // namespace fnordmetric

