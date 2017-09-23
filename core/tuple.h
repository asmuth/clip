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
#include <stdint.h>
#include <initializer_list>

namespace zdb {

class zdb_tuple {
public:

  void add_uint32(int column, uint32_t key);
  void add_uint64(int column, uint64_t key);
  void add_int32(int column, int32_t key);
  void add_int64(int column, int64_t key);
  void add_float32(int column, float key);
  void add_float64(int column, double key);
  void add_string(int column, const char* key, size_t keylen);

  bool get_bool(int column);
  uint32_t get_uint32(int column);
  uint64_t get_uint64(int column);
  int32_t get_int32(int column);
  int64_t get_int64(int column);
  float get_float32(int column);
  double get_float64(int column);
  void get_string(int column, const char* data, size_t* size);

};

} // namespace zdb

