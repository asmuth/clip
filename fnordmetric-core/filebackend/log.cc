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


Log::Log(
    const Snapshot& snapshot,
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

void Log::import(
    std::shared_ptr<MmapPageManager> mmap_manager,
    const PageManager::Page& first_log_page,
    Snapshot* snapshot) {
  PageManager::Page current_page(first_log_page);
  uint64_t cur;

  printf("import...\n");
}

}
}
