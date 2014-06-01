/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <algorithm>
#include "log.h"
#include "../fnv.h"

namespace fnordmetric {
namespace database {

LogReader::LogReader(
    std::shared_ptr<PageManager> page_manager,
    const PageManager::Page& first_log_page,
    LogSnapshot* destination) :
    page_manager_(std::move(page_manager)),
    current_page_(first_log_page),
    destination_(destination) {}

void LogReader::import() {
  bool running = true;

  destination_->last_used_byte = current_page_.offset + current_page_.size;

  while (running) {
    auto mmapped_offset = current_page_.offset;
    auto mmapped = page_manager_->getPage(current_page_);
    size_t offset = 0;

    destination_->current_log_page = current_page_;
    destination_->current_log_page_offset = offset;

    while (current_page_.offset == mmapped_offset) {
      running = importNextEntry(mmapped.get(), current_page_.size, &offset);

      if (running) {
        destination_->current_log_page_offset = offset;
      } else {
        break;
      }
    }
  }

  for (auto& stream : destination_->streams) {
    if (stream.pages_.size() > 0) {
      countPageUsedBytes(stream.pages_.back());
    }
  }
}

bool LogReader::importNextEntry(
    const PageManager::PageRef* mmapped,
    size_t mmaped_size,
    size_t* offset) {
  size_t header_size = sizeof(Log::EntryHeader);

  if (*offset + header_size >= mmaped_size) {
    return false;
  }

  auto entry_header = mmapped->structAt<Log::EntryHeader>(*offset);
  size_t entry_size = header_size + entry_header->size;

  if (entry_header->size == 0 || *offset + entry_size >= mmaped_size) {
    return false;
  }

  if (entry_header->checksum != entry_header->computeChecksum()) {
    fprintf(stderr, "warning: invalid checksum for log entry 0x%" PRIx64 "\n",
        mmapped->page_.offset + *offset);
    return false;
  }

  *offset += entry_size;
  importLogEntry(entry_header);
  return true;
}

void LogReader::importLogEntry(const Log::EntryHeader* entry) {
  switch (entry->type) {

    case Log::PAGE_ALLOC_ENTRY: {
      auto alloc_entry = (Log::PageAllocEntry *) entry;
      auto iter = streams_.find(alloc_entry->stream_id);
      LogSnapshot::StreamState* stream_state;

      if (iter == streams_.end()) {
        destination_->streams.emplace_back(alloc_entry->stream_id);
        stream_state = &destination_->streams.back();
        streams_[alloc_entry->stream_id] = stream_state;

        size_t key_len = alloc_entry->hdr.size -
            (sizeof(Log::PageAllocEntry) - sizeof(Log::EntryHeader));
        stream_state->stream_key_.insert(0, alloc_entry->stream_key, key_len);

        if (alloc_entry->stream_id > destination_->max_stream_id) {
          destination_->max_stream_id = alloc_entry->stream_id;
        }
      } else {
        stream_state = iter->second;
      }

      PageManager::Page page;
      page.offset = alloc_entry->page_offset;
      page.size = alloc_entry->page_size;

      auto alloc = new PageAlloc(page, alloc_entry->page_first_row_time);
      stream_state->pages_.push_back(std::shared_ptr<PageAlloc>(alloc));
      setLastUsedByte(page.offset + page.size);
      break;
    }

    case Log::PAGE_FINISH_ENTRY: {
      auto finish_entry = (Log::PageFinishEntry *) entry;
      auto iter = streams_.find(finish_entry->stream_id);

      if (iter != streams_.end()) {
        auto stream_state = iter->second;
        if (stream_state->pages_.back()->page_.offset ==
            finish_entry->page_offset) {
          stream_state->pages_.back()->used_ = finish_entry->page_used;
          break;
        }
      }

      fprintf(stderr, "warning: unexpected PAGE_FINISH log entry\n");
      break;
    }

    case Log::NEXT_PAGE_ENTRY: {
      auto next_entry = (Log::NextPageEntry *) entry;
      current_page_.offset = next_entry->page_offset;
      current_page_.size = next_entry->page_size;
      setLastUsedByte(current_page_.offset + current_page_.size);
      break;
    }

    default:
      fprintf(stderr, "warning: invalid log entry type %i\n", entry->type);

  };
}

void LogReader::countPageUsedBytes(std::shared_ptr<PageAlloc> page) {
  size_t offset = 0;
  size_t max = page->page_.size - sizeof(Log::EntryHeader) - 1;
  auto mmapped = page_manager_->getPage(page->page_);

  while (offset < max) {
    auto row = mmapped->structAt<RowHeader>(offset);
    auto row_size = sizeof(RowHeader) + row->size;

    if (row->size == 0 || row->time == 0) {
      return;
    }

    if (offset + row_size >= page->page_.size) {
      return;
    }

    if (row->checksum != row->computeChecksum()) {
      return;
    }

    offset += row_size;
    page->used_ = offset;
  }
}

void LogReader::setLastUsedByte(uint64_t index) {
  if (index > destination_->last_used_byte) {
    destination_->last_used_byte = index;
  }
}

const uint64_t Log::kMinLogPageSize = 512;

Log::Log(
    const LogSnapshot& snapshot,
    std::shared_ptr<PageManager> page_manager) :
    page_manager_(page_manager),
    current_page_(snapshot.current_log_page),
    current_page_offset_(snapshot.current_log_page_offset) {}

Log::Log(
    const PageManager::Page& first_log_page,
    std::shared_ptr<PageManager> page_manager) :
    page_manager_(page_manager),
    current_page_(first_log_page),
    current_page_offset_(0) {}

void Log::appendEntry(Log::PageAllocEntry entry) {
  entry.hdr.size = sizeof(PageAllocEntry) - sizeof(EntryHeader);
  entry.hdr.type = PAGE_ALLOC_ENTRY;
  appendEntry((uint8_t *) &entry, sizeof(PageAllocEntry));
}

void Log::appendEntry(Log::PageAllocEntry entry, const std::string& stream_key) {
  assert(stream_key.size() < 0xffff);
  entry.hdr.size = sizeof(PageAllocEntry) - sizeof(EntryHeader);
  entry.hdr.size += stream_key.size();
  entry.hdr.type = PAGE_ALLOC_ENTRY;
  size_t tmp_len = sizeof(PageAllocEntry) + stream_key.size();
  uint8_t* tmp = (uint8_t *) malloc(tmp_len);
  assert(tmp);
  memcpy(tmp, &entry, sizeof(PageAllocEntry));
  memcpy(tmp + sizeof(PageAllocEntry), stream_key.c_str(), stream_key.size());
  appendEntry(tmp, tmp_len);
  free(tmp);
}

void Log::appendEntry(Log::PageFinishEntry entry) {
  entry.hdr.size = sizeof(PageFinishEntry) - sizeof(EntryHeader);
  entry.hdr.type = PAGE_FINISH_ENTRY;
  appendEntry((uint8_t *) &entry, sizeof(PageFinishEntry));
}

void Log::appendEntry(Log::PageFreeEntry entry) {
  entry.hdr.size = sizeof(PageFreeEntry) - sizeof(EntryHeader);
  entry.hdr.type = PAGE_FREE_ENTRY;
  appendEntry((uint8_t *) &entry, sizeof(PageFreeEntry));
}

// FIXPAUL lock!
void Log::appendEntry(uint8_t* data, size_t length) {
  uint64_t reserved_length = length + sizeof(Log::NextPageEntry);

  /* allocate a new page if the current one is full */
  if (current_page_offset_ + reserved_length >= current_page_.size) {
    assert(current_page_offset_ + sizeof(Log::NextPageEntry) <=
        current_page_.size);

    auto next_page_size = std::max(kMinLogPageSize, reserved_length);
    auto next_page = page_manager_->allocPage(next_page_size);

    Log::NextPageEntry log_entry;
    log_entry.page_offset = next_page.offset;
    log_entry.page_size = next_page.size;
    log_entry.hdr.size = sizeof(Log::NextPageEntry) - sizeof(Log::EntryHeader);
    log_entry.hdr.type = NEXT_PAGE_ENTRY;
    log_entry.hdr.checksum = log_entry.hdr.computeChecksum();

    auto mmaped = page_manager_->getPage(current_page_);
    auto dst = mmaped->structAt<char>(current_page_offset_);
    memcpy(dst, (char *) &log_entry, sizeof(Log::NextPageEntry));
    current_page_offset_ += sizeof(Log::NextPageEntry);

    current_page_ = next_page;
    current_page_offset_ = 0;
  }

  /* memcpy the entry into the current page */
  auto entry = (Log::EntryHeader*) data;
  entry->checksum = entry->computeChecksum();
  auto mmaped = page_manager_->getPage(current_page_);
  auto dst = mmaped->structAt<char>(current_page_offset_);
  memcpy(dst, data, length);
  current_page_offset_ += length;

  // FIXPAUL msync
}

uint32_t Log::EntryHeader::computeChecksum() {
  FNV<uint32_t> fnv;

  return fnv.hash(
      (uint8_t *) (((char* ) this) + sizeof(checksum)),
      size + sizeof(EntryHeader) - sizeof(checksum));
}

LogSnapshot::LogSnapshot() : max_stream_id(0) {}

LogSnapshot::StreamState::StreamState(uint32_t stream_id) :
  stream_id_(stream_id) {}

}
}
