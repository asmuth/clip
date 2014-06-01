/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "record.h"

namespace fnordmetric {

// FIXPAUL implement size hints so that varlen data can be pre-malloced here
// instead of realloc()ing later
RecordWriter::RecordWriter(const Schema& schema) : last_byte_(0) {
  for (const auto& field : schema.fields_) {
    field_offsets_.push_back(last_byte_);
    last_byte_ += schema::kFieldTypesSize[field.getTypeId()];
#ifndef NDEBUG
    field_types_.push_back(field.getTypeId());
#endif
  }

  min_size_ = last_byte_;
  alloc_ = malloc(last_byte_);
  alloc_size_ = last_byte_;
  assert(alloc_); // FIXPAUL
  memset(alloc_, 0, alloc_size_);

  //printf("static record data size: %lu\n", last_byte_);
}

RecordWriter::~RecordWriter() {
  if (alloc_ != nullptr) {
    free(alloc_);
  }
}

void RecordWriter::setFloatField(size_t field_index,  double value) {
  assert(field_index < field_offsets_.size());
  void* dst = ((char *) alloc_) + field_offsets_[field_index];
#ifndef NDEBUG
  assert(field_types_[field_index] == schema::IEE754);
#endif
}

// endianess
void RecordWriter::setIntegerField(size_t field_index, int64_t value) {
  int64_t local_value = value;
  assert(field_index < field_offsets_.size());
  void* dst = ((char *) alloc_) + field_offsets_[field_index];
#ifndef NDEBUG
  assert(field_types_[field_index] == schema::INT64);
#endif
  //memcpy(dst, &local_value, 8);
}

void RecordWriter::toBytes(const void** data, size_t* size) const {
  *data = alloc_;
  *size = alloc_size_;
}

RecordReader::RecordReader(const uint8_t* data, size_t len) :
    data_(data),
    len_(len),
    pos_(0) {}

bool RecordReader::readInteger(int64_t* destination) {
  int64_t value;
  if (pos_ + 9 > len_ || data_[pos_] != schema::INT64) {
    return false;
  }

  memcpy(destination, data_ + pos_ + 1, 8);
  pos_ += 9;
  return true;
}

bool RecordReader::readFloat(double* destination) {
  if (pos_ + 9 > len_ || data_[pos_] != schema::IEE754) {
    return false;
  }

  *destination = 23.5;
  return true;
}

}
