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
#include <set>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void zdb_t;
typedef void zdb_cursor_t;
typedef void zdb_tuple_t;

typedef enum {
  ZDB_UINT32 = 1,
  ZDB_UINT64 = 2,
  ZDB_INT32 = 3,
  ZDB_INT64 = 4,
  ZDB_FLOAT32 = 6,
  ZDB_FLOAT64 = 7,
  ZDB_STRING = 8
} zdb_type_t;

typedef enum {
  ZDB_FETCH_AHEAD = 1,
  ZDB_FETCH_SINGLE = 2,
  ZDB_FETCH_ALL = 2
} zdb_cursor_advise_t;

zdb_t* zdb_init();
void zdb_close(zdb_t* db);

int zdb_open(const char* filename);
int zdb_commit(zdb_t* db);

int zdb_table_add(const char* table_name);
int zdb_table_delete(const char* table_name);

int zdb_column_add(
    const char* table_name,
    const char* column_name,
    zdb_type_t column_type,
    int* id = nullptr);

int zdb_column_delete(
    const char* table_name,
    int id);

int zdb_column_id(
    const char* table_name,
    const char* column_name);

int zdb_put(
    const char* table_name,
    zdb_tuple_t* tuple);

int zdb_put_raw(
    const char* table_name,
    const void** tuple,
    size_t tuple_size);

int zdb_lookup_uint32(
    zdb_t* db,
    uint32_t key,
    zdb_tuple_t* tuple,
    int* columns,
    size_t columns_count);

int zdb_lookup_uint64(
    zdb_t* db,
    uint64_t key,
    zdb_tuple_t* tuple,
    int* columns,
    size_t columns_count);

int zdb_lookup_int32(
    zdb_t* db,
    int32_t key,
    zdb_tuple_t* tuple,
    int* columns,
    size_t columns_count);

int zdb_lookup_int64(
    zdb_t* db,
    int64_t key,
    zdb_tuple_t* tuple,
    int* columns,
    size_t columns_count);

int zdb_lookup_float32(
    zdb_t* db,
    float key,
    zdb_tuple_t* tuple,
    int* columns,
    size_t columns_count);

int zdb_lookup_float64(
    zdb_t* db,
    double key,
    zdb_tuple_t* tuple,
    int* columns,
    size_t columns_count);

int zdb_lookup_string(
    zdb_t* db,
    const char* key,
    size_t keylen,
    zdb_tuple_t* tuple,
    int* columns,
    size_t columns_count);

zdb_cursor_t* zdb_cursor_init(zdb_t* db);
void zdb_cursor_close(zdb_cursor_t* cursor);

int zdb_cursor_use(zdb_cursor_t* cursor, const char* column);
int zdb_cursor_next(zdb_cursor_t* cursor);
uint32_t zdb_cursor_tell(zdb_cursor_t* cursor);
void zdb_cursor_advise(zdb_cursor_t* cursor, zdb_cursor_advise_t);

bool zdb_cursor_get_bool(zdb_cursor_t* cursor, int column);
uint32_t zdb_cursor_get_uint32(zdb_cursor_t* cursor, int column);
uint64_t zdb_cursor_get_uint64(zdb_cursor_t* cursor, int column);
int32_t zdb_cursor_get_int32(zdb_cursor_t* cursor, int column);
int64_t zdb_cursor_get_int64(zdb_cursor_t* cursor, int column);
float zdb_cursor_get_float32(zdb_cursor_t* cursor, int column);
double zdb_cursor_get_float64(zdb_cursor_t* cursor, int column);
void zdb_cursor_get_string(
    zdb_cursor_t* cursor,
    int column,
    const char* data,
    size_t* size);

int zdb_cursor_seek_position(zdb_cursor_t* cursor, uint32_t index);
int zdb_cursor_seek_primary_key_uint32(zdb_cursor_t* cursor, uint32_t key);
int zdb_cursor_seek_primary_key_uint64(zdb_cursor_t* cursor, uint64_t key);
int zdb_cursor_seek_primary_key_int32(zdb_cursor_t* cursor, int32_t key);
int zdb_cursor_seek_primary_key_int64(zdb_cursor_t* cursor, int64_t key);
int zdb_cursor_seek_primary_key_float32(zdb_cursor_t* cursor, float key);
int zdb_cursor_seek_primary_key_float64(zdb_cursor_t* cursor, double key);
int zdb_cursor_seek_primary_key_string(
    zdb_cursor_t* cursor,
    const char* key,
    size_t keylen);

zdb_tuple_t* zdb_tuple_alloc();
void zdb_tuple_free(zdb_tuple_t* tuple);
int zdb_tuple_add_uint32(zdb_tuple_t* tuple, int column, uint32_t key);
int zdb_tuple_add_uint64(zdb_tuple_t* tuple, int column, uint64_t key);
int zdb_tuple_add_int32(zdb_tuple_t* tuple, int column, int32_t key);
int zdb_tuple_add_int64(zdb_tuple_t* tuple, int column, int64_t key);
int zdb_tuple_add_float32(zdb_tuple_t* tuple, int column, float key);
int zdb_tuple_add_float64(zdb_tuple_t* tuple, int column, double key);
int zdb_tuple_add_string(
    zdb_tuple_t* tuple,
    const char* key,
    size_t keylen);

bool zdb_tuple_get_bool(zdb_tuple_t* tuple, int column);
uint32_t zdb_tuple_get_uint32(zdb_tuple_t* tuple, int column);
uint64_t zdb_tuple_get_uint64(zdb_tuple_t* tuple, int column);
int32_t zdb_tuple_get_int32(zdb_tuple_t* tuple, int column);
int64_t zdb_tuple_get_int64(zdb_tuple_t* tuple, int column);
float zdb_tuple_get_float32(zdb_tuple_t* tuple, int column);
double zdb_tuple_get_float64(zdb_tuple_t* tuple, int column);
void zdb_tuple_get_string(
    zdb_tuple_t* tuple,
    int column,
    const char* data,
    size_t* size);

const char* zdb_error(int err);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
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

class zdb {
public:

  zdb();

  int open(const std::string& filename);

  int table_add(const std::string& table_name);
  int table_delete(const std::string& table_name);

  int column_add(
      const std::string& table_name,
      const std::string& column_name,
      zdb_type_t column_type,
      int* id = nullptr);

  int column_delete(
      const std::string& table_name,
      int id);

  int column_id(
      const std::string& table_name,
      const std::string& column_name);

  int put(const std::string& table_name, const zdb_tuple& tuple);

  int put_raw(
      const std::string& table_name,
      const void** tuple,
      size_t tuple_size);

  int lookup_uint32(
      uint32_t key,
      zdb_tuple* tuple,
      const std::initializer_list<int> columns = {});

  int lookup_uint64(
      uint64_t key,
      zdb_tuple* tuple,
      const std::initializer_list<int> columns = {});

  int lookup_int32(
      int32_t key,
      zdb_tuple* tuple,
      const std::initializer_list<int> columns = {});

  int lookup_int64(
      int64_t key,
      zdb_tuple* tuple,
      const std::initializer_list<int> columns = {});

  int lookup_float32(
      float key,
      zdb_tuple* tuple,
      const std::initializer_list<int> columns = {});

  int lookup_float64(
      double key,
      zdb_tuple* tuple,
      const std::initializer_list<int> columns = {});

  int lookup_string(
      const char* key,
      size_t keylen,
      zdb_tuple* tuple,
      const std::initializer_list<int> columns = {});

  int cursor_init(
      const std::string& table_name,
      zdb_cursor* cursor);

  int commit();

  void close();

protected:
  static const size_t ERROR_MAXLEN = 4096;
  char error[ERROR_MAXLEN];
};

} // namespace zdb
#endif

