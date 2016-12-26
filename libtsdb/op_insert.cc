/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <iostream>
#include "tsdb.h"
#include "page_index.h"

namespace tsdb {

bool TSDB::insertUInt64(
    uint64_t series_id,
    uint64_t time,
    uint64_t value) {
  tsdb::Cursor cursor(tsdb::PageType::UINT64);
  if (!getCursor(series_id, &cursor, false, SEEK_NONE)) {
    return false;
  }

  if (cursor.seekTo(time)) {
    if (cursor.getTime() == time) {
      cursor.update(value);
    } else {
      cursor.insert(time, value);
    }
  } else {
    cursor.append(time, value);
  }

  return true;
}

} // namespace tsdb

