/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include "streamref.h"
#include "database.h"
#include "cursor.h"
#include "../clock.h"
#include "../fnv.h"

namespace fnordmetric {
namespace database {

StreamRef::StreamRef(
    Database* backend,
    uint64_t stream_id,
    const std::string& stream_key) :
    backend_(backend),
    stream_id_(stream_id),
    stream_key_(stream_key) {}

StreamRef::StreamRef(
    Database* backend,
    uint64_t stream_id,
    const std::string& stream_key,
    std::vector<std::shared_ptr<PageAlloc>>&& pages) :
    backend_(backend),
    stream_id_(stream_id),
    stream_key_(stream_key),
    pages_(std::move(pages)),
    num_pages_(pages_.size()) {}

// FIXPAUL hold append lock
uint64_t StreamRef::appendRow(const std::vector<uint8_t>& data) {
  uint64_t time = WallClock::getUnixMillis();
  size_t row_size = data.size() + sizeof(RowHeader);

  if (num_pages_ == 0) {
    // FIXPAUL estimate size
    auto page = backend_->page_manager_->allocPage(data.size() * 100);
    auto alloc = std::shared_ptr<PageAlloc>(new PageAlloc(page, time));

    Log::AllocEntry log_entry;
    log_entry.page_offset = alloc->page_.offset;
    log_entry.page_size = alloc->page_.size;
    log_entry.page_first_row_time = time;
    log_entry.stream_id = stream_id_;
    backend_->log_->appendEntry(log_entry, stream_key_);

    pages_.push_back(std::move(alloc));
    // FIXPAUL mem barrier
    num_pages_++;
  }

  if (pages_.back()->used_ + row_size > pages_.back()->page_.size) {
    // FIXPAUL estimate size
    auto page = backend_->page_manager_->allocPage(data.size() * 100);
    auto alloc = std::shared_ptr<PageAlloc>(new PageAlloc(page, time));

    // FIXPAUL: msync old page, then write old page finish

    Log::AllocEntry log_entry;
    log_entry.page_offset = alloc->page_.offset;
    log_entry.page_size = alloc->page_.size;
    log_entry.page_first_row_time = time;
    log_entry.stream_id = stream_id_;
    backend_->log_->appendEntry(log_entry);

    pages_.push_back(std::move(alloc));
    // FIXPAUL mem barrier
    num_pages_++;
  }

  auto page = pages_.back();
  auto mmaped = backend_->page_manager_->getPage(page->page_);
  RowHeader* row = mmaped->structAt<RowHeader>(page->used_);
  row->time = time;
  row->size = data.size();
  memcpy(row->data, data.data(), row->size);
  row->checksum = row->computeChecksum();
  // FIXPAUL mem barrier
  page->used_ += row_size;

  return time;
}

std::unique_ptr<Cursor> StreamRef::getCursor() {
  return std::unique_ptr<Cursor>(new Cursor(this));
}

uint32_t RowHeader::computeChecksum() {
  FNV<uint32_t> fnv;
  return fnv.hash(
      (uint8_t *) (((char* ) this) + sizeof(checksum)),
      size + sizeof(RowHeader) - sizeof(checksum));
}

PageAlloc::PageAlloc(const PageManager::Page& page, uint64_t time) :
    page_(page),
    time_(time),
    used_(0) {}

}
}
