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
    const PageManager::Page& first_log_page) :
    page_manager_(std::move(page_manager)),
    current_page_(first_log_page) {}

void LogReader::import(LogSnapshot* snapshot) {
  for (;;) {
    auto mmapped_offset = current_page_.offset;
    auto mmapped_size = current_page_.size;
    auto mmapped = page_manager_->getPage(current_page_);
    size_t offset = 0;

    while (current_page_.offset == mmapped_offset) {
      if (!importNextEntry(mmapped.get(), mmapped_size, &offset, snapshot)) {
        return;
      }
    }
  }
}

bool LogReader::importNextEntry(
    const PageManager::PageRef* mmapped,
    size_t mmaped_size,
    size_t* offset,
    LogSnapshot* destination) {
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
    fprintf(stderr, "warning: invalid checksum for log entry @ 0x%" PRIx64 "\n",
        mmapped->page_.offset + *offset);

    return false;
  }

  printf("import log @ %p, len: %u\n", entry_header, entry_header->size);

  *offset += entry_size;
  return true;
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
  //entry.hdr.checksum = ;

  appendEntry((uint8_t *) &entry, sizeof(AllocEntry));
}

void Log::appendEntry(Log::AllocEntry entry, const std::string& stream_key) {
  assert(stream_key.size() < 0xffff);
  entry.hdr.size = sizeof(AllocEntry) - sizeof(EntryHeader);
  entry.hdr.size += stream_key.size();
  entry.hdr.type = ALLOC_ENTRY;
  entry.hdr.checksum = entry.hdr.computeChecksum();

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

uint32_t Log::EntryHeader::computeChecksum() {
  FNV<uint32_t> fnv;

  return fnv.hash(
      (uint8_t *) (((char* ) this) + sizeof(checksum)),
      size + sizeof(EntryHeader) - sizeof(checksum));
}

}
}
