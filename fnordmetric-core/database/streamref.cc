/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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
    stream_key_(stream_key),
    num_pages_(0) {}

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

StreamPosition StreamRef::appendRow(const RecordWriter& record) {
  const void* data;
  size_t size;
  record.toBytes(&data, &size);
  append_mutex_.lock();
  uint64_t time = WallClock::getUnixMillis();
  auto pos = appendRow(data, size, time);
  append_mutex_.unlock();
  return pos;
}

StreamPosition StreamRef::appendRow(
    const RecordWriter& record,
    uint64_t time) {
  const void* data;
  size_t size;
  // FIXPAUL check that time goes forward
  record.toBytes(&data, &size);
  append_mutex_.lock();
  auto pos = appendRow(data, size, time);
  append_mutex_.unlock();
  return pos;
}

StreamPosition StreamRef::appendRow(
    const void* data,
    size_t size,
    uint64_t time) {
  size_t row_size = size + sizeof(RowHeader);
  assert(size > 0);

  pages_mutex_.lock();

  if (num_pages_ == 0) {
    auto estimated_page_size = estimatePageSize(row_size, row_size);
    auto page = backend_->page_manager_->allocPage(estimated_page_size);
    auto alloc = std::shared_ptr<PageAlloc>(new PageAlloc(page, time, 0));

    Log::PageAllocEntry log_entry;
    log_entry.page_offset = alloc->page_.offset;
    log_entry.page_size = alloc->page_.size;
    log_entry.page_first_row_time = time;
    log_entry.page_logical_offset = 0;
    log_entry.stream_id = stream_id_;
    backend_->log_->appendEntry(log_entry, stream_key_);

    pages_.push_back(std::move(alloc));
    num_pages_++;
  }

  auto last_page = pages_.back();
  if (last_page->used_ + row_size >= last_page->page_.size) {
    assert(last_page->num_rows_.load() > 0);
    auto last_page_avg_size = last_page->used_ / last_page->num_rows_;
    auto estimated_page_size = estimatePageSize(last_page_avg_size, row_size);
    auto page = backend_->page_manager_->allocPage(estimated_page_size);
    auto logical_offset = last_page->logical_offset_ + last_page->used_;
    auto alloc = std::shared_ptr<PageAlloc>(
        new PageAlloc(page, time, logical_offset));

    // FIXPAUL: msync last page

    {
      Log::PageFinishEntry log_entry;
      log_entry.page_offset = last_page->page_.offset;
      log_entry.page_size = last_page->page_.size;
      log_entry.page_used = last_page->used_;
      log_entry.stream_id = stream_id_;
      backend_->log_->appendEntry(log_entry);
    }

    {
      Log::PageAllocEntry log_entry;
      log_entry.page_offset = alloc->page_.offset;
      log_entry.page_size = alloc->page_.size;
      log_entry.page_first_row_time = time;
      log_entry.page_logical_offset = alloc->logical_offset_;
      log_entry.stream_id = stream_id_;
      backend_->log_->appendEntry(log_entry);
    }

    pages_.push_back(std::move(alloc));
    num_pages_++;
  }

  auto page = pages_.back();

  pages_mutex_.unlock();

  StreamPosition pos;
  pos.logical_offset = page->logical_offset_ + page->used_.load();
  pos.next_offset = pos.logical_offset + row_size;
  pos.unix_millis = time;

  auto mmaped = backend_->page_manager_->getPage(page->page_);
  RowHeader* row = mmaped->structAt<RowHeader>(page->used_.load());
  row->time = time;
  row->size = size;
  memcpy(row->data, data, size);
  row->checksum = row->computeChecksum();
  // FIXPAUL mem barrier
  page->used_ += row_size;
  page->num_rows_++;

  return pos;
}

std::unique_ptr<Cursor> StreamRef::getCursor() {
  return std::unique_ptr<Cursor>(new Cursor(this, backend_->page_manager_));
}

void StreamRef::accessPages(std::function<void(
    const std::vector<std::shared_ptr<PageAlloc>>&)> func) {
  pages_mutex_.lock();
  func(pages_);
  pages_mutex_.unlock();
}

uint64_t StreamRef::estimatePageSize(
    size_t last_page_avg_size,
    size_t row_size) const {
  assert(row_size < Database::kMaxPageSizeHard); // FIXPAUL
  size_t target = last_page_avg_size * Database::kTargetRowsPerPage;
  return std::max(std::min(target, Database::kMaxPageSizeSoft), row_size * 2);
}

uint32_t RowHeader::computeChecksum() {
  FNV<uint32_t> fnv;
  return fnv.hash(
      (uint8_t *) (((char* ) this) + sizeof(checksum)),
      size + sizeof(RowHeader) - sizeof(checksum));
}

PageAlloc::PageAlloc(
    const PageManager::Page& page,
    uint64_t time,
    uint64_t logical_offset) :
    page_(page),
    time_(time),
    logical_offset_(logical_offset),
    used_(0),
    num_rows_(0) { }

bool StreamPosition::operator==(const StreamPosition& other) {
  return
      unix_millis == other.unix_millis &&
      logical_offset == other.logical_offset &&
      next_offset == other.next_offset;
}

}
}
