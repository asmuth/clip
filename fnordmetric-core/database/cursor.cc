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

Cursor::Cursor(const StreamRef* stream_ref) :
    stream_ref_(stream_ref),
    page_manager_(stream_ref_->backend_->page_manager_.get()),
    current_page_index_(0),
    current_page_offset_(0),
    current_page_(nullptr),
    current_page_ref_(nullptr) {}

uint64_t Cursor::seekTo(uint64_t position) {

}

// FIXPAUL lock
uint64_t Cursor::seekToFirst() {
  // FIXPAUL mem barrier
  size_t num_pages = stream_ref_->num_pages_.load();
  if (num_pages > 0) {
    current_page_ = stream_ref_->pages_.at(0);
    current_page_ref_ = page_manager_->getPage(current_page_->page_);
    current_page_offset_ = 0;
    current_page_index_ = 0;
    return getCurrentRow()->time;
  } else {
    return 0;
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

  /* try to advance to the next page */
  size_t num_pages = stream_ref_->num_pages_.load();

  if (current_page_index_ + 1 >= num_pages) {
    return false;
  }

  current_page_index_++;
  current_page_ = stream_ref_->pages_.at(current_page_index_);
  current_page_ref_ = page_manager_->getPage(current_page_->page_);
  current_page_offset_ = 0;
  // FIXPAUL mem barrier
  return true;
}

const RowHeader* Cursor::getCurrentRow() {
  return current_page_ref_->structAt<RowHeader>(current_page_offset_);
}

void Cursor::getRow(const std::function<void (const uint8_t* data,
    size_t len, uint64_t time)>& func) const {}

}
}
