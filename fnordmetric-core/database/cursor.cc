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

StreamPosition Cursor::seekToTime(uint64_t position) {
  // FIXPAUL
  return getCurrentPosition();
}

StreamPosition Cursor::seekToFirst() {
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
    StreamPosition pos;
    pos.unix_millis = 0;
    pos.logical_offset = 0;
    pos.next_offset = 0;
    return pos;
  } else {
    current_page_ref_ = page_manager_->getPage(current_page_->page_);
    return getCurrentPosition();
  }
}

StreamPosition Cursor::seekToLast() {
  /* try to seek to the last page */
  stream_ref_->accessPages([this]
      (const std::vector<std::shared_ptr<PageAlloc>>& stream_pages) {
    if (stream_pages.size() > 0) {
      this->current_page_ = stream_pages.back();
      this->current_page_offset_ = 0;
      this->current_page_index_ = 0;
    }
  });

  if (current_page_.get() == nullptr) {
    StreamPosition pos;
    pos.unix_millis = 0;
    pos.logical_offset = 0;
    pos.next_offset = 0;
    return pos;
  } else {
    /* seek to the last row in the page */
    current_page_ref_ = page_manager_->getPage(current_page_->page_);
    while(next()) {}
    return getCurrentPosition();
  }
}

bool Cursor::next() {
  if (current_page_.get() == nullptr) {
    return false;
  }

  /* try to advance the offset in the current page */
  auto new_offset =
      current_page_offset_ + getCurrentRow()->size + sizeof(RowHeader);

  if (new_offset < current_page_->used_.load()) {
    current_page_offset_ = new_offset;
    // FIXPAUL mem barrier if tail page / still mutable
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
    return true;
  }
}

const RowHeader* Cursor::getCurrentRow() const {
  if (current_page_.get() == nullptr) {
    return nullptr;
  } else {
    auto row = current_page_ref_->structAt<RowHeader>(current_page_offset_);
    // FIXPAUL verify checksum
    return row;
  }
}

StreamPosition Cursor::getCurrentPosition() const {
  StreamPosition pos;
  if (current_page_.get() == nullptr) {
    pos.unix_millis = 0;
    pos.logical_offset = 0;
    pos.next_offset = 0;
  } else {
    auto row = current_page_ref_->structAt<RowHeader>(current_page_offset_);
    pos.unix_millis = row->time;
    pos.logical_offset = current_page_->logical_offset_ + current_page_offset_;
    pos.next_offset = pos.logical_offset + row->size + sizeof(RowHeader);
  }
  return pos;
}

}
}
