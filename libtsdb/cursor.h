/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include "transaction.h"
#include "page_buffer.h"
#include "page_map.h"

namespace tsdb {

class Cursor {
public:

  enum SeekType {
    SEEK_LOWER_BOUND,
    SEEK_UPPER_BOUND
  };

  Cursor(PageType type);
  Cursor(PageType type, PageMap* page_map, Transaction&& txn);
  Cursor(Cursor&& o);
  Cursor(const Cursor& o) = delete;
  Cursor& operator=(const Cursor& o) = delete;
  Cursor& operator=(Cursor&& o);

  bool valid();
  void get(uint64_t* timestamp, uint64_t* value);
  bool next();

  void seekTo(uint64_t timestamp, SeekType seek_type = SEEK_LOWER_BOUND);

  bool next(uint64_t* timestamp, uint64_t* value);

  void insert(uint64_t timestamp, uint64_t value);

  void update(uint64_t timestamp, uint64_t value);

protected:
  Transaction txn_;
  size_t page_pos_;
  PageMap* page_map_;
  PageBuffer page_buf_;
  bool page_buf_valid_;
  size_t page_buf_pos_;
};

} // namespace tsdb

