/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/samplefieldindex.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace metricdb  {

SampleFieldIndex* SampleFieldIndex::makeIndex() {
  return new SampleFieldIndex();
}

SampleFieldIndex::SampleFieldIndex() :
    fnord::sstable::Index(kIndexType),
    max_index_(0) {}

void SampleFieldIndex::addRow(
    size_t body_offset,
    void const* key,
    size_t key_size,
    void const* data,
    size_t data_size) const {
  /* noop */
}

uint32_t SampleFieldIndex::findLabel(const std::string& key) const {
  std::lock_guard<std::mutex> lock_holder(mutex_);

  auto iter = indexes_.find(key);
  if (iter == indexes_.end()) {
    return 0;
  } else {
    return iter->second;
  }
}

uint32_t SampleFieldIndex::addLabel(const std::string& key) {
  std::lock_guard<std::mutex> lock_holder(mutex_);

  auto iter = indexes_.find(key);
  if (iter != indexes_.end()) {
    RAISE(kIllegalStateError, "label already exists in index");
  }

  auto new_idx = ++max_index_;
  indexes_.emplace(key, new_idx);

  return new_idx;
}


}
}

