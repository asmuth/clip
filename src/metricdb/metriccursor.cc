/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/metriccursor.h>
#include <fnordmetric/util/stringutil.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace metricdb {

MetricCursor::MetricCursor(
    std::shared_ptr<MetricSnapshot> snapshot) :
    snapshot_(snapshot),
    table_index_(0) {}

bool MetricCursor::next() {
  if (tableCursor()->next()) {
    return true;
  }

  if (table_index_ < (snapshot_->tables().size() - 1)) {
    table_cur_ = snapshot_->tables()[++table_index_]->cursor();
    return table_cur_->valid();
  }

  return false;
}

bool MetricCursor::valid() {
  return tableCursor()->valid();
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

SampleReader* MetricCursor::sample() {
  void* data;
  size_t data_size;
  tableCursor()->getData(&data, &data_size);

  sample_.reset(new SampleReader(data, data_size, nullptr));
  return sample_.get();
}

fnord::sstable::Cursor* MetricCursor::tableCursor() {
  if (table_cur_.get() == nullptr) {
    table_cur_ = snapshot_->tables()[table_index_]->cursor();
  }

  return table_cur_.get();
}

}
}
