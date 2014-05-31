/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FILEBACKEND_LOG_H
#define _FNORDMETRIC_FILEBACKEND_LOG_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "streamref.h"

namespace fnordmetric {
namespace database {

struct LogSnapshot {
  LogSnapshot();
  struct StreamState {
    StreamState(uint32_t stream_id);
    uint64_t stream_id_;
    std::string stream_key_;
    std::vector<std::shared_ptr<PageAlloc>> pages_;
  };
  std::vector<PageManager::Page> free_pages;
  std::vector<StreamState> streams;
  uint32_t max_stream_id;
  PageManager::Page current_log_page;
  uint64_t current_log_page_offset;
  uint64_t last_used_byte;
};

class Log {
  friend class DatabaseTest;
public:
  struct __attribute__((__packed__)) EntryHeader {
    uint32_t checksum;
    uint16_t type;
    uint16_t size;
    uint32_t computeChecksum();
  };

  struct __attribute__((__packed__)) AllocEntry {
    EntryHeader hdr;
    uint32_t stream_id;
    uint64_t page_offset;
    uint32_t page_size;
    uint64_t page_first_row_time;
    char stream_key[];
  };

  enum kEntryTypes {
    ALLOC_ENTRY = 0x01
  };

  explicit Log(
      const LogSnapshot& snapshot,
      std::shared_ptr<PageManager> page_manager);

  explicit Log(
      const PageManager::Page& first_log_page,
      std::shared_ptr<PageManager> page_manager);

  Log(const Log& copy) = delete;
  Log& operator=(const Log& copy) = delete;
  Log(const Log&& move);

  void appendEntry(AllocEntry entry);
  void appendEntry(AllocEntry entry, const std::string& stream_key);

protected:
  void appendEntry(uint8_t* data, size_t length);

  std::shared_ptr<PageManager> page_manager_;
  PageManager::Page current_page_;
  uint64_t current_page_offset_;
};

/**
 * This is an internal class. For usage instructions and extended documentation
 * please refer to "storagebackend.h" and "database.h"
 */
class LogReader {
public:
  LogReader(
      std::shared_ptr<PageManager> page_manager,
      const PageManager::Page& first_log_page,
      LogSnapshot* destination);

  /**
   * Import the log. Returns a snapshot of the imported log
   */
  void import();

protected:

  /**
   * Import a single log entry. Returns true if there might be another entry
   * in the log and returns false when the last entry was read
   */
  bool importNextEntry(
      const PageManager::PageRef* mmapped,
      size_t mmaped_size,
      size_t* offset);

  /**
   * Import a single log entry
   */
  void importLogEntry(const Log::EntryHeader* entry);

  /**
   * Count the number of used byte in a page
   */
  void countPageUsedBytes(std::shared_ptr<PageAlloc> page);

  std::unordered_map<uint32_t, LogSnapshot::StreamState*> streams_;
  const std::shared_ptr<PageManager> page_manager_;
  PageManager::Page current_page_;
  LogSnapshot* destination_;
};

}
}
#endif
