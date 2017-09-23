/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
 */
#pragma once
#include "tuple.h"
#include "metadata.h"

namespace zdb {

struct database {

  database(metadata&& meta, bool readonly);
  database(const database& o) = delete;
  database(database&& o) = delete;
  ~database();

  void close();

  int cursor_init(
      const std::string& table_name,
      cursor_ref* cursor);

  int commit();

  metadata meta;
  const bool readonly;
  pthread_rwlock_t lock;
};

} // namespace zdb

