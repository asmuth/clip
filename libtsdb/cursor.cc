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
#include "assert.h"

namespace tsdb {

Cursor::Cursor(
    PageType type) :
    page_pos_(0),
    page_id_(-1),
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
    page_id_(-1),
    page_map_(page_map),
    page_buf_(type),
    page_buf_valid_(false),
    page_buf_pos_(0) {}

Cursor::Cursor(Cursor&& o) :
    txn_(std::move(o.txn_)),
    page_pos_(o.page_pos_),
    page_id_(o.page_id_),
    page_map_(o.page_map_),
    page_buf_(std::move(o.page_buf_)),
    page_buf_valid_(o.page_buf_valid_),
    page_buf_pos_(o.page_buf_pos_) {
  o.page_pos_ = 0;
  o.page_id_ = -1;
  o.page_map_ = nullptr;
  o.page_buf_valid_ = false;
  o.page_buf_pos_ = 0;
}

Cursor& Cursor::operator=(Cursor&& o) {
  txn_ = std::move(o.txn_);
  page_pos_ = o.page_pos_;
  page_id_ = o.page_id_;
  page_map_ = o.page_map_;
  page_buf_ = std::move(o.page_buf_);
  page_buf_valid_ = o.page_buf_valid_;
  page_buf_pos_ = o.page_buf_pos_;

  o.page_pos_ = 0;
  o.page_id_ = -1;
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

uint64_t Cursor::getTime() {
  uint64_t timestamp;
  page_buf_.getTimestamp(page_buf_pos_, &timestamp);
  return timestamp;
}

bool Cursor::next(uint64_t* timestamp, uint64_t* value) {
  if (!valid()) {
    return false;
  }

  get(timestamp, value);
  next();
  return true;
}

bool Cursor::seekTo(uint64_t timestamp) {
  if (!page_map_) {
    return false;
  }

  /* search for the correct page */
  auto page_idx = txn_.getPageIndex();
  page_pos_ = 0; // FIXME: binary search for the correct page
  page_id_ = page_idx->getEntries()[page_pos_].page_id;

  /* load the page */
  if (page_map_->getPage(page_id_, &page_buf_)) {
    page_buf_valid_ = true;
  } else {
    page_buf_valid_ = false;
    return false;
  }

  /* search for the correct slot in the page */
  if (timestamp == 0) {
    page_buf_pos_ = 0;
    return true;
  }

  {
    uint64_t high = page_buf_.getSize();
    uint64_t low = 0;

    uint64_t actual_timestamp;
    while (high != low) {
      page_buf_pos_ = (high + low) / 2 ;
      page_buf_.getTimestamp(page_buf_pos_, &actual_timestamp);
      if (actual_timestamp > timestamp) {
        high = page_buf_pos_;
      } else if (actual_timestamp < timestamp) {
        low = page_buf_pos_ + 1;
      } else {
        return true;
      }
    }

    if (actual_timestamp < timestamp) {
      page_buf_pos_++;
    }
  }

  return page_buf_pos_ < page_buf_.getSize();
}

bool Cursor::seekToFirst() {
  /* open the first page */
  auto page_idx = txn_.getPageIndex();
  if (!page_buf_valid_ || page_pos_ != 0) {
    page_pos_ = 0;
    page_id_ = page_idx->getEntries()[page_pos_].page_id;

    if (page_map_->getPage(page_id_, &page_buf_)) {
      page_buf_valid_ = true;
    } else {
      page_buf_valid_ = false;
      return false;
    }
  }

  /* seek to the first value in the page */
  page_buf_pos_ = 0;
  return true;
}

bool Cursor::seekToLast() {
  /* open the last page */
  auto page_idx = txn_.getPageIndex();
  auto page_pos = page_idx->getSize() - 1;

  if (!page_buf_valid_ || page_pos_ != page_pos) {
    page_pos_ = page_pos;
    page_id_ = page_idx->getEntries()[page_pos_].page_id;

    if (page_map_->getPage(page_id_, &page_buf_)) {
      page_buf_valid_ = true;
    } else {
      page_buf_valid_ = false;
      return false;
    }
  }

  /* seek to the last value in the page */
  page_buf_pos_ = page_buf_.getSize() - 1;
  return true;
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
    if (page_pos_ + 1 >= page_idx->getSize()) {
      return false;
    }

    page_id_ = page_idx->getEntries()[++page_pos_].page_id;
    if (!page_map_->getPage(page_id_, &page_buf_)) {
      page_buf_valid_ = false;
      return false;
    }

    page_buf_valid_ = true;
    page_buf_pos_ = 0;
    return true;
  }
}

void Cursor::update(uint64_t value) {
  assert(!txn_.isReadonly());
  assert(valid());

  auto modify_fn = [this, value] (PageBuffer* page) -> bool {
    page->update(page_buf_pos_, &value, sizeof(value));
    return true;
  };

  txn_.getPageMap()->modifyPage(
      txn_.getPageIndex()->getType(),
      page_id_,
      modify_fn);

  modify_fn(&page_buf_);
}

void Cursor::insert(uint64_t timestamp, uint64_t value) {
  assert(!txn_.isReadonly());
  assert(valid());

  /* append the value */
  auto modify_fn = [this, timestamp, value] (PageBuffer* page) -> bool {
    page->insert(page_buf_pos_, timestamp, &value, sizeof(value));
    return true;
  };

  txn_.getPageMap()->modifyPage(
      txn_.getPageIndex()->getType(),
      page_id_,
      modify_fn);

  modify_fn(&page_buf_);
}

void Cursor::append(uint64_t timestamp, uint64_t value) {
  assert(!txn_.isReadonly());

  /* open the last page */
  auto page_idx = txn_.getPageIndex();
  auto last_page_pos = page_idx->getSize() - 1;

  if (!page_buf_valid_ || page_pos_ != last_page_pos) {
    page_pos_ = last_page_pos;
    page_id_ = page_idx->getEntries()[page_pos_].page_id;

    if (page_map_->getPage(page_id_, &page_buf_)) {
      page_buf_valid_ = true;
    } else {
      page_buf_valid_ = false;
      return;
    }
  }

  /* append the value */
  auto modify_fn = [this, timestamp, value] (PageBuffer* page) -> bool {
    page->append(timestamp, &value, sizeof(value));
    return true;
  };

  txn_.getPageMap()->modifyPage(
      txn_.getPageIndex()->getType(),
      page_id_,
      modify_fn);

  modify_fn(&page_buf_);

  /* seek to the inserted value */
  page_buf_pos_ = page_buf_.getSize() - 1;
}

} // namespace tsdb

