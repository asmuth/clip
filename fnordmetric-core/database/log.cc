/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include "log.h"

namespace fnordmetric {
namespace filebackend {

LogReader::LogReader(
    std::shared_ptr<MmapPageManager> mmap_manager,
    const PageManager::Page& first_log_page) :
    mmap_manager_(std::move(mmap_manager)),
    current_page_(first_log_page) {}

void LogReader::import(Snapshot* snapshot) {
  printf("import!\n");
  for (;;)
    auto mmaped_offset = current_page_.offset;
    auto mmaped = mmap_manager_->getPage(current_page_);
    size_t offset = 0;

    while (current_page_.offset == mapped_offset) {
      if (!importNextEntry(*mmaped, current_page_.size, &offset, snapshot)) {
        return;
      }
    }
  }
}

Log::Log(
    const LogReader::Snapshot& snapshot,
    std::shared_ptr<PageManager> page_manager,
    std::shared_ptr<MmapPageManager> mmap_manager) :
    page_manager_(page_manager),
    mmap_manager_(mmap_manager),
    current_page_(snapshot.current_log_page),
    current_page_offset_(snapshot.current_log_page_offset) {}

Log::Log(
    const PageManager::Page& first_log_page,
    std::shared_ptr<PageManager> page_manager,
    std::shared_ptr<MmapPageManager> mmap_manager) :
    page_manager_(page_manager),
    mmap_manager_(mmap_manager),
    current_page_(first_log_page),
    current_page_offset_(0) {}

void Log::appendEntry(Log::AllocEntry entry) {
  entry.hdr.length = sizeof(AllocEntry) - sizeof(EntryHeader);
  entry.hdr.type = ALLOC_ENTRY;
  //entry.hdr.checksum = ;

  appendEntry((uint8_t *) &entry, sizeof(AllocEntry));
}

void Log::appendEntry(Log::AllocEntry entry, const std::string& stream_key) {
  assert(stream_key.size() < 0xffff);
  entry.hdr.length = sizeof(AllocEntry) - sizeof(EntryHeader);
  entry.hdr.length += stream_key.size();
  entry.hdr.type = ALLOC_ENTRY;
  //entry.hdr.checksum = ;

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
  printf("append to log, %llu\n", current_page_.offset);

  if (current_page_offset_ + length >= current_page_.size) {
    // FIXPAUL
  }

  auto mmaped = mmap_manager_->getPage(current_page_);
  memcpy(mmaped.structAt<char>(current_page_offset_), data, length);
  current_page_offset_ += length;
}

}
}
