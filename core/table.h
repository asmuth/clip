/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace zdb {

struct ColumnConfig {

};

class TableSchema {
public:

  ReturnCode addColumn(const std::string& name, const ColumnConfig& config);
  ReturnCode removeColumn(const std::string& name);
  bool fetchColumnID(const std::string& name, uint32_t* id);
  bool fetchColumnConfig(const std::string& name, const ColumnConfig** config);

protected:

};

using TableMap = std::map<std::string, TableSchema>;

} // namespace zdb

