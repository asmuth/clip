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
#include <string>
#include <memory>
#include <assert.h>
#include "pagemanager.h"
#include "streamref.h"

namespace fnordmetric {
namespace filebackend {

/**
 * This is an internal class. For usage instructions and extended documentation
 * please refer to "storagebackend.h" and "filebackend.h"
 */
class Log {
  friend class FileBackendTest;
public:
  struct Snapshot {
    std::vector<PageManager::Page> free_pages;
    std::vector<std::shared_ptr<StreamRef>> streams;
    PageManager::Page current_log_page;
    uint64_t current_log_page_offset;
    uint64_t last_used_byte;
  };

  /**
   * Import the log. Returns a snapshot of the imported log
   */
  static void import(
      std::shared_ptr<MmapPageManager> mmap_manager,
      const PageManager::Page& first_log_page,
      Snapshot* snapshot);

  explicit Log(
      const Snapshot& snapshot,
      std::shared_ptr<PageManager> page_manager,
      std::shared_ptr<MmapPageManager> mmap_manager);

  explicit Log(
      const PageManager::Page& first_log_page,
      std::shared_ptr<PageManager> page_manager,
      std::shared_ptr<MmapPageManager> mmap_manager);

  Log(const Log& copy) = delete;
  Log& operator=(const Log& copy) = delete;
  Log(const Log&& move);


protected:
  std::shared_ptr<PageManager> page_manager_;
  std::shared_ptr<MmapPageManager> mmap_manager_;
  PageManager::Page current_page_;
  uint64_t current_page_offset_;
};


}
}
#endif
