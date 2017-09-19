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
#ifdef __cplusplus
#include <zdb/util/return_code.h>
#include <zdb/table.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void zdb_t;
typedef void zdb_cursor_t;
typedef unsigned zdb_column_id;

typedef enum {
  UINT32, UINT64, INT32, INT64, FLOAT32, FLOAT64, STRING
} zdb_type_t;

typedef enum {
  ZDB_FETCH_AHEAD = 1,
  ZDB_FETCH_SINGLE = 2,
  ZDB_FETCH_ALL = 2
} zdb_cursor_advise;

typedef enum {
  ZDB_SUCCESS = 0
} zdb_error_t;

zdb_t* zdb_init();
zdb_error_t zdb_open(const char* filename);

zdb_cursor_t* zdb_cursor_t_init(zdb_t* db);

const char* zdb_error(zdb_error_t err);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
namespace zdb {

class zdb_cursor {
public:

  void advise(zdb_cursor_advise);
  Result<zdb_column_id> use(const std::string& column);

  bool get_bool(zdb_column_id column);
  uint32_t get_uint32(zdb_column_id column);
  uint64_t get_uint64(zdb_column_id column);
  int32_t get_int32(zdb_column_id column);
  int64_t get_int64(zdb_column_id column);
  float get_float32(zdb_column_id column);
  double get_float64(zdb_column_id column);
  void get_string(zdb_column_id column, const char* data, size_t* size);

  zdb_error_t next();
  uint32_t tell() const;

  zdb_error_t seek_position(uint32_t index);
  zdb_error_t seek_primary_key_uint32(uint32_t key);
  zdb_error_t seek_primary_key_uint64(uint64_t key);
  zdb_error_t seek_primary_key_int32(int32_t key);
  zdb_error_t seek_primary_key_int64(int64_t key);
  zdb_error_t seek_primary_key_float32(float key);
  zdb_error_t seek_primary_key_float64(double key);
  zdb_error_t seek_primary_key_string(const char* key, size_t keylen);

};

class zdb_tuple {
public:

  void add_uint32(zdb_column_id column, uint32_t key);
  void add_uint64(zdb_column_id column, uint64_t key);
  void add_int32(zdb_column_id column, int32_t key);
  void add_int64(zdb_column_id column, int64_t key);
  void add_float32(zdb_column_id column, float key);
  void add_float64(zdb_column_id column, double key);
  void add_string(zdb_column_id column, const char* key, size_t keylen);

};

class zdb {
public:

  zdb();

  zdb_error_t open(const std::string& filename);

  zdb_error_t table_add(const std::string& table_name);
  zdb_error_t table_delete(const std::string& table_name);

  zdb_error_t column_add(
      const std::string& table_name,
      const std::string& column_name,
      zdb_type_t column_type,
      zdb_column_id* id = nullptr);

  zdb_error_t column_delete(
      const std::string& table_name,
      zdb_column_id id);

  zdb_error_t column_id(
      const std::string& table_name,
      const std::string& column_name,
      zdb_column_id* id);

  zdb_error_t put(const std::string& table_name, const zdb_tuple& tuple);

  zdb_error_t put_raw(
      const std::string& table_name,
      const void** tuple,
      size_t tuple_size);

  zdb_error_t cursor_init(
      const std::string& table_name,
      zdb_cursor* cursor);

  zdb_error_t commit();

  void close();

protected:
  static const size_t ERROR_MAXLEN = 4096;
  char error[ERROR_MAXLEN];
};

} // namespace zdb
#endif

