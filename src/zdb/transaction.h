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
#include <initializer_list>
#include "snapshot.h"
#include "tuple.h"
#include "cursor.h"

namespace zdb {

class transaction {
public:

  transaction(snapshot_ref snap, bool readonly);
  transaction(const transaction& o) = delete;
  transaction(transaction&& o);
  transaction& operator=(const transaction& o) = delete;
  transaction& operator=(transaction&& o);
  ~transaction();

  int cursor_init(
      const std::string& table_name,
      cursor_ref* cursor);

  int commit();

protected:
  snapshot_ref snap;
  bool readonly;
};

} // namespace zdb

