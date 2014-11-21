/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/backends/disk/metriccursor.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace metricdb {
namespace disk_backend {

MetricCursor::MetricCursor(
    std::shared_ptr<MetricSnapshot> snapshot,
    TokenIndex* token_index) :
    snapshot_(snapshot),
    token_index_(token_index),
    table_index_(0) {}

bool MetricCursor::next() {
  if (tableCursor()->next()) {
    return true;
  }


  while (table_index_ < (snapshot_->tables().size() - 1)) {
    table_cur_ = snapshot_->tables()[++table_index_]->cursor();

    if (table_cur_->valid()) {
      return true;
    }
  }

  return false;
}

bool MetricCursor::valid() {
  if (tableCursor()->valid()) {
    return true;
  }

  while (table_index_ < (snapshot_->tables().size() - 1)) {
    table_cur_ = snapshot_->tables()[++table_index_]->cursor();

    if (table_cur_->valid()) {
      return true;
    }
  }

  return false;
}

uint64_t MetricCursor::time() {
  uint64_t time = 0;

  void* key;
  size_t key_len;
  tableCursor()->getKey(&key, &key_len);

  if (key_len == sizeof(time)) {
    memcpy(&time, key, sizeof(time));
  } else {
    RAISE(kIllegalStateError, "invalid key");
  }
  return time;
}

fnord::sstable::Cursor* MetricCursor::tableCursor() {
  if (table_cur_.get() == nullptr) {
    table_cur_ = snapshot_->tables()[table_index_]->cursor();
  }

  return table_cur_.get();
}

}
}
}

