/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <mutex>
#include <map>
#include <vector>
#include "zdb.h"
#include "page.h"

namespace zdb {

struct column_info {
  uint16_t id;
  std::string name;
  zdb_type_t type;
};

struct column_block {
  column_block();
  bool present;
  page_buf* page;
};

using column_list = std::vector<column_info>;

struct row_block {
  row_block(const column_list& table);
  std::vector<column_block> columns;
  uint64_t row_count;
};

struct table {
  column_list columns;
  std::vector<row_block> row_map;
  uint64_t row_count;
};

struct metadata {
  metadata();

  bool dirty;
  std::map<std::string, table> tables;
};

} // namespace zdb

