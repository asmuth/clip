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
    DataFrameType type,
    bool has_time) :
    type_(type),
    has_time_(has_time),
    data_(nullptr),
    size_(0),
    capacity_(0) {}

DataFrame::DataFrame(
    DataFrame&& other) :
    type_(other.type_),
    has_time_(other.has_time_),
    id_(other.id_),
    data_(other.data_),
    size_(other.size_),
    capacity_(other.capacity_) {
  other.id_.clear();
  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;
}

DataFrame& DataFrame::operator=(DataFrame&& other) {
  if (data_) {
    free(data_);
  }

  type_ = other.type_;
  has_time_ = other.has_time_;
  id_ = other.id_;
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

const std::vector<std::string> DataFrame::getID() const {
  return id_;
}

void DataFrame::setID(const std::vector<std::string> id) {
  id_ = id;
}

const uint64_t* DataFrame::getTime() const {
  auto ptr = static_cast<const char*>(data_) + getEntrySize() * getSize();
  return (const uint64_t*) (ptr);
}

uint64_t* DataFrame::getTime() {
  auto ptr = static_cast<char*>(data_) + getEntrySize() * getSize();
  return (uint64_t*) ptr;
}

bool DataFrame::hasTime() const {
  return has_time_;
}

size_t DataFrame::getSize() const {
  return size_;
}

size_t DataFrame::getEntrySize() const {
  switch (type_) {
    case DataFrameType::UINT: return sizeof(uint64_t);
    case DataFrameType::INT: return sizeof(int64_t);
    case DataFrameType::DOUBLE: return sizeof(double);
  }
}

void DataFrame::resize(size_t new_size) {
  auto old_size = size_;
  size_ = new_size;
  if (new_size <= capacity_) {
    return;
  } else {
    capacity_ = size_; // FIXME
  }

  auto esize = getEntrySize();
  if (has_time_) {
    esize += sizeof(uint64_t);
  }

  auto old_data = data_;
  data_ = malloc(capacity_ * esize);

  if (!data_) { // this should never happen ;)
    perror("malloc() failed");
    abort();
  }

  if (old_data) {
    memcpy(data_, old_data, old_size * getEntrySize());
  }

  if (old_data && has_time_) {
    memcpy(
        (char*) data_ + size_ * getEntrySize(),
        (const char*) old_data + old_size * getEntrySize(),
        old_size * sizeof(uint64_t));
  }
}

void DataFrame::debugPrint() const {
  auto id = StringUtil::join(id_, ", ");
  fprintf(
      stderr,
      "==== DATA FRAME: %p; id=%s, size=%lu ====\n",
      this,
      id.c_str(),
      size_);

  for (size_t i = 0; i < size_; ++i) {
    if (has_time_) {
      fprintf(stderr, ">> time: %llu ", getTime()[i]);
    } else {
      fprintf(stderr, ">> ");
    }

    switch (type_) {
      case DataFrameType::UINT:
        fprintf(stderr, "val: " PRIu64 "\n", getData<uint64_t>()[i]);
        break;
      case DataFrameType::INT:
        fprintf(stderr, "val: " PRId64 "\n", getData<int64_t>()[i]);
        break;
      case DataFrameType::DOUBLE:
        fprintf(stderr, "val: %f\n", getData<double>()[i]);
        break;
    }
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

DataFrame* DataFrameBundle::addFrame(DataFrameType type, bool has_time) {
  frames_.emplace_back(type, has_time);
  return &frames_.back();
}

void DataFrameBundle::debugPrint() const {
  for (size_t i = 0; i < frames_.size(); ++i) {
    frames_[i].debugPrint();
  }
}

} // namespace fnordmetric

