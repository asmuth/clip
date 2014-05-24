/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include "memorybackend.h"

namespace fnordmetric {

MemoryBackend::Cursor::Cursor(
    const std::shared_ptr<std::vector<IStorageCursor::RowType>>& data) :
    data_(data),
    pos_(-1) {}

uint64_t MemoryBackend::Cursor::seekTo(uint64_t position) {
  // FIXPAUL implement me
  return 0;
}

uint64_t MemoryBackend::Cursor::seekToFirst() {
  if (data_->size() > 0) {
    pos_ = 0;
    return std::get<0>(data_->at(pos_));
  } else {
    return 0;
  }
}

uint64_t MemoryBackend::Cursor::seekToLast() {
  if (data_->size() > 0) {
    pos_ = data_->size() - 1;
    return std::get<0>(data_->at(pos_));
  } else {
    return 0;
  }
}

void MemoryBackend::Cursor::getRow(const GetRowCB& func) const {
  if (pos_ < 0 || pos_ >= data_->size()) {
    return;
  }

  auto row = data_->at(pos_);
  auto data = std::get<1>(row).data();
  auto len = std::get<1>(row).size();
  auto time = std::get<0>(row);

  func(data, len, time);
}

bool MemoryBackend::Cursor::next() {
  if (pos_ < data_->size() - 1) {
    ++pos_;
    return true;
  } else {
    return false;
  }
}

uint64_t MemoryBackend::Cursor::appendRow(const std::vector<uint8_t>& data) {
  uint64_t now = WallClock::getUnixMillis();
  data_->emplace_back(std::make_pair(now, data));
  printf("append_row time=%llu, size=%lu\n", now, data.size());
  return now;
}

std::unique_ptr<IStorageCursor>MemoryBackend::Cursor::clone() const {
  return std::unique_ptr<IStorageCursor>(new Cursor(data_, pos_));
}

MemoryBackend::Cursor::Cursor(
    const std::shared_ptr<std::vector<IStorageCursor::RowType>>& data,
    int pos) :
    data_(data),
    pos_(pos) {}

std::unique_ptr<MemoryBackend> MemoryBackend::New() {
  return std::unique_ptr<MemoryBackend>(new MemoryBackend());
}

std::unique_ptr<IStorageCursor> MemoryBackend::getCursor(
    const std::string& key) {
  std::unique_ptr<IStorageCursor> cursor_ptr;
  auto iter = streams_.find(key);

  if (iter == streams_.end()) {
    std::shared_ptr<std::vector<IStorageCursor::RowType>> data_ptr(
      new std::vector<IStorageCursor::RowType>());

    streams_.insert(std::make_pair(key, data_ptr));
    cursor_ptr.reset(new Cursor(data_ptr));
  } else {
    cursor_ptr.reset(new Cursor(iter->second));
  }

  return cursor_ptr;
}

}
