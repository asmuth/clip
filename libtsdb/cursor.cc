/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "cursor.h"
#include "page_index.h"
#include <iostream>

namespace tsdb {

Cursor::Cursor(
    PageType type) :
    page_pos_(0),
    page_map_(nullptr),
    page_buf_(type),
    page_buf_valid_(false),
    page_buf_pos_(0) {}

Cursor::Cursor(
    PageType type,
    PageMap* page_map,
    Transaction&& txn) :
    txn_(std::move(txn)),
    page_pos_(0),
    page_map_(page_map),
    page_buf_(type),
    page_buf_valid_(false),
    page_buf_pos_(0) {
  seekTo(0);
}

Cursor::Cursor(Cursor&& o) :
    txn_(std::move(o.txn_)),
    page_pos_(o.page_pos_),
    page_map_(o.page_map_),
    page_buf_(std::move(o.page_buf_)),
    page_buf_valid_(o.page_buf_valid_),
    page_buf_pos_(o.page_buf_pos_) {
  o.page_pos_ = 0;
  o.page_map_ = nullptr;
  o.page_buf_valid_ = false;
  o.page_buf_pos_ = 0;
}

Cursor& Cursor::operator=(Cursor&& o) {
  txn_ = std::move(o.txn_);
  page_pos_ = o.page_pos_;
  page_map_ = o.page_map_;
  page_buf_ = std::move(o.page_buf_);
  page_buf_valid_ = o.page_buf_valid_;
  page_buf_pos_ = o.page_buf_pos_;

  o.page_pos_ = 0;
  o.page_map_ = nullptr;
  o.page_buf_valid_ = false;
  o.page_buf_pos_ = 0;
  return *this;
}

bool Cursor::valid() {
  return page_map_ && page_buf_valid_ && page_buf_pos_ < page_buf_.getSize();
}

void Cursor::get(uint64_t* timestamp, uint64_t* value) {
  page_buf_.getTimestamp(page_buf_pos_, timestamp);
  page_buf_.getValue(page_buf_pos_, value);
}

bool Cursor::next(uint64_t* timestamp, uint64_t* value) {
  if (!valid()) {
    return false;
  }

  get(timestamp, value);
  next();
  return true;
}

void Cursor::seekTo(uint64_t timestamp) {
  /* search for the correct page */
  auto page_idx = txn_.getPageIndex();
  page_pos_ = 0; // FIXME: binary search for the correct page

  /* load the page */
  auto page_id = page_idx->getEntries()[page_pos_].page_id;
  if (page_map_->getPage(page_id, &page_buf_)) {
    page_buf_valid_ = true;
  } else {
    page_buf_valid_ = false;
    return;
  }

  /* search for the correct slot in the page */
  if (timestamp == 0) {
    page_buf_pos_ = 0;
    return;
  }

  {
    auto high = page_buf_.getSize();
    auto low = 0;

    uint64_t actual_timestamp;
    while (high != low) {
      page_buf_pos_ = (high + low) / 2 ;
      page_buf_.getTimestamp(page_buf_pos_, &actual_timestamp);
      if (actual_timestamp > timestamp) {
        high = page_buf_pos_;
      } else if (actual_timestamp < timestamp) {
        low = page_buf_pos_ + 1;
      } else {
        return;
      }
    }

    if (actual_timestamp < timestamp) {
      page_buf_pos_++;
    }
  }
}

bool Cursor::next() {
  if (!page_map_) {
    return false;
  }

  if (page_buf_valid_ && page_buf_pos_ < page_buf_.getSize()) {
    ++page_buf_pos_;
    return true;
  } else {
    /* load next page */
    auto page_idx = txn_.getPageIndex();
    if (page_pos_ >= page_idx->getSize()) {
      return false;
    }

    auto page_id = page_idx->getEntries()[page_pos_].page_id;
    if (!page_map_->getPage(page_id, &page_buf_)) {
      return false;
    }

    ++page_pos_;
    page_buf_valid_ = true;
    page_buf_pos_ = 0;
    return true;
  }
}

} // namespace tsdb

