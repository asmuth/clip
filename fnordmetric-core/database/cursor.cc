/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include "cursor.h"
#include "streamref.h"
#include "database.h"

namespace fnordmetric {
namespace database {

Cursor::Cursor(
    StreamRef* stream_ref,
    std::shared_ptr<PageManager> page_manager) :
    stream_ref_(stream_ref),
    page_manager_(page_manager),
    current_page_index_(0),
    current_page_offset_(0),
    current_page_(nullptr),
    current_page_ref_(nullptr) {}

uint64_t Cursor::seekTo(uint64_t position) {

}

uint64_t Cursor::seekToFirst() {
  /* try to seek to the first page */
  stream_ref_->accessPages([this]
      (const std::vector<std::shared_ptr<PageAlloc>>& stream_pages) {
    if (stream_pages.size() > 0) {
      this->current_page_ = stream_pages.at(0);
      this->current_page_offset_ = 0;
      this->current_page_index_ = 0;
    }
  });

  if (current_page_.get() == nullptr) {
    return 0;
  } else {
    current_page_ref_ = page_manager_->getPage(current_page_->page_);
    return getCurrentRow()->time;
  }
}

uint64_t Cursor::seekToLast() {}

bool Cursor::next() {
  if (current_page_.get() == nullptr) {
    return false;
  }

  /* try to advance the offset in the current page */
  auto new_offset =
      current_page_offset_ + getCurrentRow()->size + sizeof(RowHeader);

  if (new_offset < current_page_->used_.load()) {
    current_page_offset_ = new_offset;
    // FIXPAUL mem barrier if tail page
    return true;
  }

  auto old_page_index = current_page_index_;

  /* try to advance to the next page */
  stream_ref_->accessPages([this]
      (const std::vector<std::shared_ptr<PageAlloc>>& stream_pages) {
    if (this->current_page_index_ + 1 < stream_pages.size()) {
      this->current_page_index_++;
      this->current_page_ = stream_pages.at(current_page_index_);
      this->current_page_offset_ = 0;
    }
  });

  if (current_page_index_ == old_page_index) {
    return false;
  } else {
    current_page_ref_ = page_manager_->getPage(current_page_->page_);
    // FIXPAUL mem barrier
    return true;
  }
}

const RowHeader* Cursor::getCurrentRow() {
  return current_page_ref_->structAt<RowHeader>(current_page_offset_);
}

void Cursor::getRow(const std::function<void (const uint8_t* data,
    size_t len, uint64_t time)>& func) const {}

}
}
