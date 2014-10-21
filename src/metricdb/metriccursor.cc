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
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace metricdb {

MetricCursor::MetricCursor(
    std::shared_ptr<MetricSnapshot> snapshot) :
    snapshot_(snapshot) {}

bool MetricCursor::next() {
  return tableCursor()->next();
}

bool MetricCursor::valid() {
  return tableCursor()->valid();
}

uint64_t MetricCursor::time() {
  return 0;
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
    // FIXPAUL start with first table, not always the live table
    table_cur_ = snapshot_->liveTable()->getCursor();
  }

  return table_cur_.get();
}

}
}
