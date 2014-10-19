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
#include <stdlib.h>

namespace fnordmetric {
namespace metricdb {

SampleWriter::SampleWriter(
    SampleFieldIndex* label_index) :
    label_index_(label_index),
    ptr_(calloc(1, kInitialDataSize)),
    size_(kInitialDataSize),
    used_(sizeof(BinaryFormat::SampleHeader)) {}

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

    // write label definition
  } else {
    // write label reference
  }

  // write label value
}

void* SampleWriter::data() const {
  return ptr_;
}

size_t SampleWriter::size() const {
  return used_;
}

}
}

