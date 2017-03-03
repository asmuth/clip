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
#include <fnordmetric/table.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace fnordmetric {

class TableMap {
friend class TableMapBuilder;
friend class TableListCursor;
public:

  ~TableMap();

  TableConfig* findTable(const std::string& key) const;

  std::set<std::string> listTables() const;

protected:

  using SlotType = std::pair<TableConfig*, bool>;
  using MapType = std::map<std::string, SlotType>;
  using IterType = MapType::iterator;

  MapType metrics_;
  std::shared_ptr<TableMap> next_;
};

class TableMapBuilder {
public:

  TableMapBuilder(TableMap* metric_map);

  void addTable(
      const std::string& key,
      std::unique_ptr<TableConfig> metric);

  TableConfig* findTable(const std::string& key);

  std::shared_ptr<TableMap> getTableMap();

protected:
  std::shared_ptr<TableMap> metric_map_;
};

class VersionedTableMap {
public:

  VersionedTableMap();

  std::shared_ptr<TableMap> getTableMap() const;

  void updateTableMap(std::shared_ptr<TableMap> metric_map);

protected:
  mutable std::mutex mutex_;
  std::shared_ptr<TableMap> metric_map_;
};

class TableListCursor {
public:

  TableListCursor(std::shared_ptr<TableMap> metric_map);
  TableListCursor(const TableListCursor& o) = delete;
  TableListCursor(TableListCursor&& o);
  TableListCursor& operator=(const TableListCursor& o) = delete;

  const std::string& getTableID();

  bool isValid() const;
  bool next();

protected:
  std::shared_ptr<TableMap> metric_map_;
  TableMap::IterType begin_;
  TableMap::IterType cur_;
  TableMap::IterType end_;
};

} // namespace fnordmetric

