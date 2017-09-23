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

namespace zdb {

class zdb_cursor {
public:

  void advise(zdb_cursor_advise_t);
  int use(const std::string& column);

  bool get_bool(int column);
  uint32_t get_uint32(int column);
  uint64_t get_uint64(int column);
  int32_t get_int32(int column);
  int64_t get_int64(int column);
  float get_float32(int column);
  double get_float64(int column);
  void get_string(int column, const char* data, size_t* size);

  int next();
  uint32_t tell() const;

  int seek_position(uint32_t index);
  int seek_primary_key_uint32(uint32_t key);
  int seek_primary_key_uint64(uint64_t key);
  int seek_primary_key_int32(int32_t key);
  int seek_primary_key_int64(int64_t key);
  int seek_primary_key_float32(float key);
  int seek_primary_key_float64(double key);
  int seek_primary_key_string(const char* key, size_t keylen);

};

//class Cursor {
//public:
//
//  Cursor();
//  Cursor(PageMap* page_map, Transaction&& txn);
//  Cursor(Cursor&& o);
//  Cursor(const Cursor& o) = delete;
//  Cursor& operator=(const Cursor& o) = delete;
//  Cursor& operator=(Cursor&& o);
//
//  bool valid();
//
//  void get(uint64_t* timestamp, void* value, size_t value_len);
//  uint64_t getTime();
//  void getValue(void* value, size_t value_len);
//
//  /* seek to the first entry that is greater or equal */
//  bool seekTo(uint64_t timestamp);
//  bool seekToFirst();
//  bool seekToLast();
//
//  bool next();
//  bool next(uint64_t* timestamp, void* value, size_t value_len);
//
//  /* insert at the current position (new value will appear before current
//     value at this position after insert) */
//  void insert(uint64_t timestamp, const void* value, size_t value_len);
//
//  void append(uint64_t timestamp, const void* value, size_t value_len);
//
//  void update(const void* value, size_t value_len);
//
//protected:
//  Transaction txn_;
//  size_t page_pos_;
//  size_t page_id_;
//  PageMap* page_map_;
//  PageBuffer page_buf_;
//  bool page_buf_valid_;
//  size_t page_buf_pos_;
//};

} // namespace zdb

