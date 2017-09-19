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

  Cursor();
  Cursor(PageMap* page_map, Transaction&& txn);
  Cursor(Cursor&& o);
  Cursor(const Cursor& o) = delete;
  Cursor& operator=(const Cursor& o) = delete;
  Cursor& operator=(Cursor&& o);

  bool valid();

  void get(uint64_t* timestamp, void* value, size_t value_len);
  uint64_t getTime();
  void getValue(void* value, size_t value_len);

  /* seek to the first entry that is greater or equal */
  bool seekTo(uint64_t timestamp);
  bool seekToFirst();
  bool seekToLast();

  bool next();
  bool next(uint64_t* timestamp, void* value, size_t value_len);

  /* insert at the current position (new value will appear before current
     value at this position after insert) */
  void insert(uint64_t timestamp, const void* value, size_t value_len);

  void append(uint64_t timestamp, const void* value, size_t value_len);

  void update(const void* value, size_t value_len);

protected:
  Transaction txn_;
  size_t page_pos_;
  size_t page_id_;
  PageMap* page_map_;
  PageBuffer page_buf_;
  bool page_buf_valid_;
  size_t page_buf_pos_;
};

} // namespace tsdb

