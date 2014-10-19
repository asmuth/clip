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
#include <fnordmetric/metricdb/samplewriter.h>
#include <fnordmetric/metricdb/binaryformat.h>
#include <fnordmetric/util/runtimeexception.h>
#include <stdlib.h>

namespace fnordmetric {
namespace metricdb {

SampleWriter::SampleWriter(
    SampleFieldIndex* label_index) :
    label_index_(label_index),
    ptr_(malloc(kInitialDataSize)),
    size_(kInitialDataSize),
    used_(sizeof(BinaryFormat::SampleHeader)) {
  if (ptr_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }
}

SampleWriter::~SampleWriter() {
  free(ptr_);
}

void SampleWriter::writeValue(uint64_t value) {
  static_cast<BinaryFormat::SampleHeader*>(ptr_)->value = value;
}

void SampleWriter::writeLabel(
    const std::string& key,
    const std::string& value) {
  uint32_t indexed_key = label_index_->findLabel(key);

  if (indexed_key == 0) {
    indexed_key = label_index_->addLabel(key);
    uint32_t def_marker = 0xffffffff;
    append(&def_marker, sizeof(def_marker));
    append(&indexed_key, sizeof(indexed_key));
    uint32_t key_len = key.size();
    append(&key_len, sizeof(key_len));
    append(key.c_str(), key.size());
  } else {
    append(&indexed_key, sizeof(indexed_key));
  }

  uint32_t value_len = value.size();
  append(&value_len, sizeof(value_len));
  append(value.c_str(), value.size());
}

void* SampleWriter::data() const {
  return ptr_;
}

size_t SampleWriter::size() const {
  return used_;
}

void SampleWriter::append(void const* data, size_t size) {
  size_t resize = size_;

  while (used_ + size >= size_) {
    resize *= 2;
  }

  if (resize > size_) {
    auto new_ptr = realloc(ptr_, resize);

    if (ptr_ == nullptr) {
      RAISE(kMallocError, "realloc() failed");
    }

    ptr_ = new_ptr;
  }

  memcpy(((char*) ptr_) + used_, data, size);
  used_ += size;
}

}
}

