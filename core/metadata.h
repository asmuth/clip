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

namespace zdb {
class metadata;
class page_map;

struct column {
  uint16_t id;
  std::string name;
  zdb_type_t type;
};

struct table {
  std::vector<column> columns;
};

struct metadata {
  metadata();
  metadata(metadata&& o);
  metadata(const metadata& o) = delete;
  metadata& operator=(const metadata& o) = delete;
  metadata& operator=(metadata&& o);
  ~metadata();

  bool dirty;
  std::map<std::string, table> tables;
};

using metadata_ref = std::shared_ptr<metadata>;

} // namespace zdb

