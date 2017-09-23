/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
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

