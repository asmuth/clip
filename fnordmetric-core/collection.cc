/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include "collection.h"
#include "pagemanager.h"
#include "transaction.h"

namespace fnordmetric {

Collection::Collection(
    const Schema& schema,
    std::shared_ptr<PageManager> page_manager) :
    schema_(schema),
    page_manager_(std::move(page_manager)) {}


Collection::~Collection() {}

std::unique_ptr<Transaction> Collection::startTransaction() {
  auto transaction = new Transaction(this);
  return std::unique_ptr<Transaction>(transaction);
}

void Collection::writeRootPage(const PageManager::Page& root_page) {
  PageManager::Page header_page;
  header_page.offset = 0;
  header_page.size = kMinReservedHeaderSize;

  auto header_mmap = page_manager_->getPage(header_page);
  auto file_header = header_mmap->structAt<FileHeader>(0);

  file_header->root_page_offset = root_page.offset;
  file_header->root_page_size = root_page.size;
  // FIXPAUL msync header
}


const Schema& Collection::getSchema() const {
  return schema_;
}

std::shared_ptr<PageManager> Collection::getPageManager() const {
  return page_manager_;
}

}
