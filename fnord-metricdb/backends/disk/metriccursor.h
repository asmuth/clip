/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_METRICDB_DISK_BACKEND_METRICCURSOR_H
#define _FNORD_METRICDB_DISK_BACKEND_METRICCURSOR_H
#include <fnord-metricdb/backends/disk/metricsnapshot.h>
#include <fnord-metricdb/backends/disk/samplereader.h>
#include <fnord-base/util/binarymessagereader.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnord {
namespace metric_service {
namespace disk_backend {

class MetricCursor {
public:
  MetricCursor(
      std::shared_ptr<MetricSnapshot> snapshot,
      TokenIndex* token_index);

  MetricCursor(const MetricCursor& copy) = delete;
  MetricCursor& operator=(const MetricCursor& copy) = delete;

  bool next();
  bool valid();

  void seekToMostRecentSample();
  uint64_t time();

  template <typename T>
  SampleReader<T>* sample();

protected:
  std::shared_ptr<MetricSnapshot> snapshot_;
  int table_index_;
  fnord::sstable::Cursor* tableCursor();
  std::unique_ptr<fnord::sstable::Cursor> table_cur_;
  std::unique_ptr<fnord::util::BinaryMessageReader> sample_;
  TokenIndex* token_index_;
};

// impl
template <typename T>
SampleReader<T>* MetricCursor::sample() {
  void* data;
  size_t data_size;
  tableCursor()->getData(&data, &data_size);

  auto reader = new SampleReader<T>(data, data_size, token_index_);
  sample_.reset(reader);
  return reader;
}

}
}
}

#endif

