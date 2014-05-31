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

// FIXPAUL hold append lock
uint64_t StreamRef::appendRow(const std::vector<uint8_t>& data) {
  uint64_t time = WallClock::getUnixMillis();
  size_t row_size = data.size() + sizeof(RowHeader);

  printf("row-size: %llu, %llu\n", row_size, data.size());
  if (pages_.size() == 0) {
    PageAlloc alloc;
    // FIXPAUL estimate size
    alloc.page = backend_->page_manager_->allocPage(data.size() * 100);
    alloc.used = 0;
    alloc.t0   = time;
    alloc.t1   = time;

    Log::AllocEntry log_entry;
    log_entry.page_offset = alloc.page.offset;
    log_entry.page_size = alloc.page.size;
    log_entry.page_first_row_time = time;
    log_entry.stream_id = stream_id_;
    backend_->log_->appendEntry(log_entry, stream_key_);

    pages_.push_back(std::move(alloc));
  }

  if (pages_.back().used + row_size > pages_.back().page.size) {
    PageAlloc alloc;
    // FIXPAUL estimate size
    alloc.page = backend_->page_manager_->allocPage(data.size() * 100);
    alloc.used = 0;
    alloc.t0   = time;
    alloc.t1   = time;

    Log::AllocEntry log_entry;
    log_entry.page_offset = alloc.page.offset;
    log_entry.page_size = alloc.page.size;
    log_entry.page_first_row_time = time;
    log_entry.stream_id = stream_id_;
    backend_->log_->appendEntry(log_entry);

    pages_.push_back(std::move(alloc));
  }

  printf("mmap page for writing @ %llu\n", pages_.back().page.offset);
  auto mmaped = backend_->mmap_manager_->getPage(pages_.back().page);
  RowHeader* row = mmaped.structAt<RowHeader>(pages_.back().used);
  row->time = time;
  row->size = data.size();
  memcpy(row->data, data.data(), row->size);
  row->computeChecksum();
  pages_.back().used += row_size;

  return time;
}

void StreamRef::RowHeader::computeChecksum() {
  FNV<uint64_t> fnv;
  checksum = fnv.hash(
      (uint8_t *) (((char* ) this) + sizeof(checksum)),
      size + sizeof(time) + sizeof(size));
}

StreamDescriptor::StreamDescriptor(std::shared_ptr<StreamRef> stream_ref) :
  stream_ref_(stream_ref) {}

uint64_t StreamDescriptor::appendRow(const std::vector<uint8_t>& data) {
  return stream_ref_->appendRow(data);
}

std::unique_ptr<IBackend::IStreamCursor>StreamDescriptor::getCursor() {
  return std::unique_ptr<Cursor>(new Cursor(stream_ref_));
}

}
}
