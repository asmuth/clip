/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include "fnord-mdb/MDBCursor.h"

namespace fnord {
namespace mdb {

MDBCursor::MDBCursor(
    MDB_cursor* mdb_cur) :
    mdb_cur_(mdb_cur),
    is_closed_(false) {
  incRef();
}

void MDBCursor::close() {
  if (is_closed_) {
    RAISE(kRuntimeError, "cursor was already closed");
  }

  is_closed_ = true;
  mdb_cursor_close(mdb_cur_);
  decRef();
}

bool MDBCursor::getFirst(Buffer* rkey, Buffer* rvalue) {
  void* key;
  size_t key_size;
  void* val;
  size_t val_size;

  if (getFirst(&key, &key_size, &val, &val_size)) {
    *rkey = Buffer(key, key_size);
    *rvalue = Buffer(val, val_size);
    return true;
  } else {
    return false;
  }
}

bool MDBCursor::getFirst(
    void** key,
    size_t* key_size,
    void** value,
    size_t* value_size) {
  MDB_val mkey, mval;
  auto rc = mdb_cursor_get(mdb_cur_, &mkey, &mval, MDB_FIRST);
  if (rc == MDB_NOTFOUND) {
    return false;
  }

  if (rc != 0) {
    auto err = String(mdb_strerror(rc));
    RAISEF(kRuntimeError, "mdb_cursor_get(FIRST) failed: $0", err);
  }

  *key = mkey.mv_data;
  *key_size = mkey.mv_size;
  *value = mval.mv_data;
  *value_size = mval.mv_size;
  return true;
}

bool MDBCursor::getNext(Buffer* rkey, Buffer* rvalue) {
  void* key;
  size_t key_size;
  void* val;
  size_t val_size;

  if (getNext(&key, &key_size, &val, &val_size)) {
    *rkey = Buffer(key, key_size);
    *rvalue = Buffer(val, val_size);
    return true;
  } else {
    return false;
  }
}

bool MDBCursor::getNext(
    void** key,
    size_t* key_size,
    void** value,
    size_t* value_size) {
  MDB_val mkey, mval;
  auto rc = mdb_cursor_get(mdb_cur_, &mkey, &mval, MDB_NEXT);
  if (rc == MDB_NOTFOUND) {
    return false;
  }

  if (rc != 0) {
    auto err = String(mdb_strerror(rc));
    RAISEF(kRuntimeError, "mdb_cursor_get(NEXT) failed: $0", err);
  }

  *key = mkey.mv_data;
  *key_size = mkey.mv_size;
  *value = mval.mv_data;
  *value_size = mval.mv_size;
  return true;
}

}
}
