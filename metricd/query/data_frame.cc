/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V. <laura@eventql.io>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/query/data_frame.h"
#include <string.h>
#include <assert.h>
#include <inttypes.h>

namespace fnordmetric {

DataFrame::DataFrame(
    tval_type type) :
    type_(type),
    data_(nullptr),
    size_(0),
    capacity_(0) {}

DataFrame::DataFrame(
    DataFrame&& other) :
    type_(other.type_),
    id_(other.id_),
    tags_(other.tags_),
    data_(other.data_),
    size_(other.size_),
    capacity_(other.capacity_) {
  other.id_.clear();
  other.tags_.clear();
  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;
}

DataFrame& DataFrame::operator=(DataFrame&& other) {
  if (data_) {
    free(data_);
  }

  type_ = other.type_;
  id_ = other.id_;
  tags_ = other.tags_;
  data_ = other.data_;
  size_ = other.size_;
  capacity_ = other.capacity_;

  other.id_.clear();
  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;

  return *this;
}

DataFrame::~DataFrame() {
  if (data_) {
    free(data_);
  }
}

tval_type DataFrame::getType() const {
  return type_;
}

const std::string& DataFrame::getID() const {
  return id_;
}

void DataFrame::setID(const std::string& id) {
  id_ = id;
}

const std::set<std::string>& DataFrame::getTags() const {
  return tags_;
}

void DataFrame::addTag(const std::string& tag) {
  tags_.insert(tag);
}

const uint64_t* DataFrame::getTime(size_t idx /* = 0 */) const {
  auto ptr = static_cast<const char*>(data_) + sizeof(uint64_t) * idx;
  return (const uint64_t*) (ptr);
}

uint64_t* DataFrame::getTime(size_t idx /* = 0 */) {
  auto ptr = static_cast<char*>(data_) + sizeof(uint64_t) * idx;
  return (uint64_t*) ptr;
}

const void* DataFrame::getData(size_t idx /* = 0 */) const {
  auto ptr =
      static_cast<const char*>(data_) +
      sizeof(uint64_t) * getSize() +
      tval_len(type_) * idx;

  return ptr;
}

void* DataFrame::getData(size_t idx /* = 0 */) {
  auto ptr =
      static_cast<char*>(data_) +
      sizeof(uint64_t) * getSize() +
      tval_len(type_) * idx;

  return ptr;
}

size_t DataFrame::getSize() const {
  return size_;
}

void DataFrame::resize(size_t new_size) {
  auto old_size = size_;
  size_ = new_size;
  if (new_size <= capacity_) {
    return;
  } else {
    capacity_ = size_; // FIXME
  }

  auto esize = tval_len(type_) + sizeof(uint64_t);

  auto old_data = data_;
  data_ = malloc(capacity_ * esize);

  if (!data_) { // this should never happen ;)
    perror("malloc() failed");
    abort();
  }

  if (old_data) {
    memcpy(data_, old_data, old_size * tval_len(type_));

    memcpy(
        (char*) data_ + size_ * tval_len(type_),
        (const char*) old_data + old_size * tval_len(type_),
        old_size * sizeof(uint64_t));
  }
}

void DataFrame::addValue(uint64_t time, const void* data, size_t data_len) {
  assert(data_len == tval_len(type_));
  auto idx = getSize();
  resize(idx + 1);
  *getTime(idx) = time;
  memcpy(getData(idx), data, std::min(data_len, tval_len(type_)));
}

void DataFrame::debugPrint() const {
  fprintf(
      stderr,
      "==== DATA FRAME: %p; id=%s, size=%lu ====\n",
      this,
      id_.c_str(),
      size_);

  for (size_t i = 0; i < size_; ++i) {
    fprintf(stderr, ">> time: %llu ", getTime()[i]);

    //switch (type_) {
    //  case DataFrameType::UINT:
    //    fprintf(stderr, "val: " PRIu64 "\n", getData<uint64_t>()[i]);
    //    break;
    //  case DataFrameType::INT:
    //    fprintf(stderr, "val: " PRId64 "\n", getData<int64_t>()[i]);
    //    break;
    //  case DataFrameType::DOUBLE:
    //    fprintf(stderr, "val: %f\n", getData<double>()[i]);
    //    break;
    //}
  }

  printf("\n\n");
}

DataFrameBundle::DataFrameBundle() {}

DataFrameBundle::DataFrameBundle(
    DataFrameBundle&& other) :
    frames_(std::move(other.frames_)) {}

DataFrameBundle& DataFrameBundle::operator=(DataFrameBundle&& other) {
  frames_ = std::move(other.frames_);
  return *this;
}

size_t DataFrameBundle::getFrameCount() const {
  return frames_.size();
}

const DataFrame* DataFrameBundle::getFrame(size_t idx) const {
  assert(idx < frames_.size());
  return &frames_[idx];
}

DataFrame* DataFrameBundle::addFrame(tval_type type) {
  frames_.emplace_back(type);
  return &frames_.back();
}

void DataFrameBundle::debugPrint() const {
  for (size_t i = 0; i < frames_.size(); ++i) {
    frames_[i].debugPrint();
  }
}

} // namespace fnordmetric

