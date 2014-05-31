/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
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
  for (;;) {
    auto mmapped_offset = current_page_.offset;
    auto mmapped = page_manager_->getPage(current_page_);
    size_t offset = 0;

    while (current_page_.offset == mmapped_offset) {
      if (!importNextEntry(mmapped.get(), current_page_.size, &offset)) {
        return;
      }
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

  importLogEntry(entry_header);
  *offset += entry_size;
  return true;
}

void LogReader::importLogEntry(const Log::EntryHeader* entry) {
  switch (entry->type) {

    case Log::ALLOC_ENTRY: {
      auto alloc_entry = (Log::AllocEntry *) entry;
      auto iter = streams_.find(alloc_entry->stream_id);
      LogSnapshot::StreamState* stream_state;

      if (iter == streams_.end()) {
        destination_->streams.emplace_back(alloc_entry->stream_id);
        stream_state = &destination_->streams.back();
        streams_[alloc_entry->stream_id] = stream_state;

        size_t key_len = alloc_entry->hdr.size -
            (sizeof(Log::AllocEntry) - sizeof(Log::EntryHeader));
        stream_state->stream_key_.insert(0, alloc_entry->stream_key, key_len);

        if (alloc_entry->stream_id > destination_->max_stream_id) {
          destination_->max_stream_id = alloc_entry->stream_id;
        }
      } else {
        stream_state = iter->second;
      }

      StreamRef::PageAlloc page;
      page.page.offset = alloc_entry->page_offset;
      page.page.size = alloc_entry->page_size;
      page.time = alloc_entry->page_first_row_time;
      stream_state->pages_.push_back(page);
      break;
    }

    default:
      fprintf(stderr, "warning: invalid log entry type %i\n", entry->type);

  };
}

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

void Log::appendEntry(Log::AllocEntry entry) {
  entry.hdr.size = sizeof(AllocEntry) - sizeof(EntryHeader);
  entry.hdr.type = ALLOC_ENTRY;
  appendEntry((uint8_t *) &entry, sizeof(AllocEntry));
}

void Log::appendEntry(Log::AllocEntry entry, const std::string& stream_key) {
  assert(stream_key.size() < 0xffff);
  entry.hdr.size = sizeof(AllocEntry) - sizeof(EntryHeader);
  entry.hdr.size += stream_key.size();
  entry.hdr.type = ALLOC_ENTRY;
  size_t tmp_len = sizeof(AllocEntry) + stream_key.size();
  uint8_t* tmp = (uint8_t *) malloc(tmp_len);
  assert(tmp);
  memcpy(tmp, &entry, sizeof(AllocEntry));
  memcpy(tmp + sizeof(AllocEntry), stream_key.c_str(), stream_key.size());
  appendEntry(tmp, tmp_len);
  free(tmp);
}

// FIXPAUL lock!
void Log::appendEntry(uint8_t* data, size_t length) {
  auto entry = (Log::EntryHeader*) data;
  entry->checksum = entry->computeChecksum();

  if (current_page_offset_ + length >= current_page_.size) {
    // FIXPAUL
  }

  auto mmaped = page_manager_->getPage(current_page_);
  auto dst = mmaped->structAt<char>(current_page_offset_);
  memcpy(dst, data, length);
  current_page_offset_ += length;
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
