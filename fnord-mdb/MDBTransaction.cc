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
#include "fnord-mdb/MDBTransaction.h"

namespace fnord {
namespace mdb {

MDBTransaction::MDBTransaction(
    MDB_txn* mdb_txn,
    MDB_dbi mdb_handle) :
    mdb_txn_(mdb_txn),
    mdb_handle_(mdb_handle),
    is_commited_(false) {}

MDBTransaction::~MDBTransaction() {
  assert(is_commited_);
  mdb_txn_abort(mdb_txn_);
}

void MDBTransaction::commit() {
  if (is_commited_) {
    RAISE(kRuntimeError, "transaction was already commited or aborted");
  }

  is_commited_ = true;

  auto rc = mdb_txn_commit(mdb_txn_);
  if (rc != 0) {
    auto err = String(mdb_strerror(rc));
    RAISEF(kRuntimeError, "mdb_txn_commit() failed: $0", err);
  }
}

void MDBTransaction::abort() {
  if (is_commited_) {
    RAISE(kRuntimeError, "transaction was already commited or aborted");
  }

  is_commited_ = true;
  mdb_txn_abort(mdb_txn_);
}

Option<Buffer> MDBTransaction::get(const Buffer& key) {
  void* data;
  size_t data_size;

  if (get(key.data(), key.size(), &data, &data_size)) {
    return Buffer(data, data_size);
  } else {
    return None<Buffer>();
  }
}

Option<Buffer> MDBTransaction::get(const String& key) {
  void* data;
  size_t data_size;

  if (get(key.c_str(), key.length(), &data, &data_size)) {
    return Some(Buffer(data, data_size));
  } else {
    return None<Buffer>();
  }
}

bool MDBTransaction::get(
    const void* key,
    size_t key_size,
    void** value,
    size_t* value_size) {
  MDB_val mkey, mval;
  mkey.mv_data = const_cast<void*>(key);
  mkey.mv_size = key_size;

  auto rc = mdb_get(mdb_txn_, mdb_handle_, &mkey, &mval);

  if (rc == MDB_NOTFOUND) {
    return false;
  }

  if (rc != 0) {
    auto err = String(mdb_strerror(rc));
    RAISEF(kRuntimeError, "mdb_get() failed: $0", err);
  }

  *value = mval.mv_data;
  *value_size = mval.mv_size;
  return true;
}

void MDBTransaction::put(const String& key, const String& value) {
  put(key.c_str(), key.length(), value.c_str(), value.length());
}

void MDBTransaction::put(const String& key, const Buffer& value) {
  put(key.c_str(), key.length(), value.data(), value.size());
}

void MDBTransaction::put(const Buffer& key, const Buffer& value) {
  put(key.data(), key.size(), value.data(), value.size());
}

void MDBTransaction::put(
    const void* key,
    size_t key_size,
    const void* value,
    size_t value_size) {
  MDB_val mkey, mval;
  mkey.mv_data = const_cast<void*>(key);
  mkey.mv_size = key_size;
  mval.mv_data = const_cast<void*>(value);
  mval.mv_size = value_size;

  auto rc = mdb_put(mdb_txn_, mdb_handle_, &mkey, &mval, 0);
  if (rc != 0) {
    auto err = String(mdb_strerror(rc));
    RAISEF(kRuntimeError, "mdb_put() failed: $0", err);
  }
}

}
}
