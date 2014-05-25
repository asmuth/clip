/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "filebackend.h"
#include "cursor.h"
#include "streamref.h"
#include "pagemanager.h"

namespace fnordmetric {
namespace filebackend {

FileBackend::FileBackend() : max_stream_id_(0), IStorageBackend() {}

std::unique_ptr<FileBackend> FileBackend::openFile(const std::string& filename) {
  auto backend = new FileBackend();
  return std::unique_ptr<FileBackend>(backend);
}

std::unique_ptr<IStorageCursor> FileBackend::getCursor(const std::string& key) {
  return std::unique_ptr<Cursor>(new Cursor(getStreamRef(key)));
}

// FIXPAUL locking!
std::shared_ptr<StreamRef> FileBackend::getStreamRef(const std::string& key) {
  auto stream_id = getStreamId(key);
  auto iter = stream_refs_.find(stream_id);

  if (iter == stream_refs_.end()) {
    std::shared_ptr<StreamRef> stream_ref(new StreamRef(stream_id, key));
    stream_refs_.insert(std::make_pair(stream_id, stream_ref));
    return stream_ref;
  } else {
    return iter->second;
  }
}

// FIXPAUL must hold lock to call!
uint64_t FileBackend::getStreamId(const std::string& stream_key) {
  auto iter = stream_ids_.find(stream_key);

  if (iter == stream_ids_.end()) {
    auto stream_id = ++max_stream_id_;
    stream_ids_.insert(std::make_pair(stream_key, stream_id));
    return stream_id;
  } else {
    return iter->second;
  }
}

Cursor::Cursor(std::shared_ptr<StreamRef> stream_ref) :
    stream_ref_(stream_ref) {}

uint64_t Cursor::seekTo(uint64_t position) {

}

uint64_t Cursor::seekToFirst() {}
uint64_t Cursor::seekToLast() {}
void Cursor::getRow(const std::function<void (const uint8_t* data,
    size_t len, uint64_t time)>& func) const {}
bool Cursor::next() {}
uint64_t Cursor::appendRow(const std::vector<uint8_t>& data) {}
std::unique_ptr<IStorageCursor> Cursor::clone() const {}

StreamRef::StreamRef(
    uint64_t stream_id,
    const std::string& stream_key) :
    stream_id_(stream_id),
    stream_key_(stream_key) {}



PageManager::PageManager(size_t end_pos, size_t block_size) :
  end_pos_(end_pos), block_size_(block_size) {}


// FIXPAUL hold lock!
const PageManager::Page PageManager::getPage(size_t min_size) {
  PageManager::Page page;

  uint64_t num_blocks = (min_size + block_size_ - 1) / block_size_;

  // FIXPAUL check freelist
  page.offset = end_pos_;
  page.size   = num_blocks * block_size_;
  page.used   = 0;
  end_pos_   += page.size;

  return page;
}

void PageManager::yieldPage(const PageManager::Page& page) {

}

}
}
