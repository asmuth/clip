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
namespace database {

LogReader::LogReader(
    std::shared_ptr<PageManager> page_manager,
    const PageManager::Page& first_log_page) :
    page_manager_(std::move(page_manager)),
    current_page_(first_log_page) {}

void LogReader::import(LogSnapshot* snapshot) {
  //for (;;) {
    auto mmapped_offset = current_page_.offset;
    auto mmapped = page_manager_->getPage(current_page_);
    size_t offset = 0;

    //while (current_page_.offset == mmapped_offset) {
    //  if (!importNextEntry(*mmapped, current_page_.size, &offset, snapshot)) {
    //    return;
    //  }
    //}
  //}
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
  if (current_page_offset_ + length >= current_page_.size) {
    // FIXPAUL
  }

  auto mmaped = page_manager_->getPage(current_page_);
  auto dst = mmaped->structAt<char>(current_page_offset_);
  memcpy(dst, data, length);
  current_page_offset_ += length;
}

}
}
