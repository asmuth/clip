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
 // FIXPAUL
}

// endianess
void RecordWriter::setIntegerField(size_t field_index, int64_t value) {
  int64_t local_value = value;
  assert(field_index < field_offsets_.size());
  void* dst = ((char *) alloc_) + field_offsets_[field_index];
#ifndef NDEBUG
  assert(field_types_[field_index] == schema::INT64);
#endif
  memcpy(dst, &local_value, 8);
}

void RecordWriter::setStringField(
    size_t field_index,
    const char* value_ptr,
    size_t value_len) {
  assert(field_index < field_offsets_.size());
#ifndef NDEBUG
  assert(field_types_[field_index] == schema::STRING);
#endif
  auto data_offset = allocVarlen(value_len);
  auto meta_ptr = (uint32_t*) (((char *) alloc_) + field_offsets_[field_index]);
  meta_ptr[0] = data_offset;
  meta_ptr[1] = value_len;
  memcpy(((char *) alloc_) + data_offset, value_ptr, value_len);
}

void RecordWriter::toBytes(const void** data, size_t* size) const {
  *data = alloc_;
  *size = last_byte_;
}

uint32_t RecordWriter::allocVarlen(uint32_t size) {
  uint32_t offset = last_byte_;
  last_byte_ += size;

  // FIXPAUL realloc in larger increments
  if (last_byte_ > alloc_size_) {
    alloc_size_ = last_byte_;
    alloc_ = realloc(alloc_, alloc_size_);
    assert(alloc_);
  }

  return offset;
}

RecordReader::RecordReader(const Schema& schema) : data_(nullptr) {
  size_t last_byte = 0;
  for (const auto& field : schema.fields_) {
    field_offsets_.push_back(last_byte);
    last_byte += schema::kFieldTypesSize[field.getTypeId()];
#ifndef NDEBUG
    field_types_.push_back(field.getTypeId());
#endif
  }
}

int64_t RecordReader::getIntegerField(
    const void* data,
    size_t field_index) const {
  assert(field_index < field_offsets_.size());
  auto src = (int64_t *) (((char *) data) + field_offsets_[field_index]);
#ifndef NDEBUG
  assert(field_types_[field_index] == schema::INT64);
#endif
  return *src;
}

void RecordReader::getStringField(
    const void* data,
    size_t field_index,
    char** str_ptr,
    size_t* str_len) const {
  assert(field_index < field_offsets_.size());
#ifndef NDEBUG
  assert(field_types_[field_index] == schema::STRING);
#endif
  auto meta_ptr = (int32_t *) (((char *) data) + field_offsets_[field_index]);
  *str_ptr = ((char *) data) + meta_ptr[0];
  *str_len = meta_ptr[1];
}

}
