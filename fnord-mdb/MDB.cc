/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-mdb/MDB.h"

namespace fnord {
namespace mdb {

RefPtr<MDB> MDB::open(const String& path) {
  MDB_env* mdb_env;

  if (mdb_env_create(&mdb_env) != 0) {
    RAISE(kRuntimeError, "mdb_env_create() failed");
  }

  auto rc = mdb_env_open(mdb_env, path.c_str(), 0, 0664);
  if (rc != 0) {
    auto err = String(mdb_strerror(rc));
    RAISEF(kRuntimeError, "mdb_env_open($0) failed: $1", path, err);
    mdb_env_close(mdb_env);
  }

  RefPtr<MDB> mdb(new MDB(mdb_env));
  mdb->openDBHandle();
  return mdb;
}

MDB::MDB(MDB_env* mdb_env) : mdb_env_(mdb_env) {}

MDB::~MDB() {
  mdb_env_close(mdb_env_);
}

RefPtr<MDBTransaction> MDB::startTransaction(bool readonly /* = false */) {
  MDB_txn* txn;

  unsigned int flags = 0;
  if (readonly) {
    flags |= MDB_RDONLY;
  }

  auto rc = mdb_txn_begin(mdb_env_, NULL, flags, &txn);
  if (rc != 0) {
    auto err = String(mdb_strerror(rc));
    RAISEF(kRuntimeError, "mdb_txn_begin() failed: $0", err);
  }

  return RefPtr<MDBTransaction>(new MDBTransaction(txn));
}

void MDB::openDBHandle() {
  MDB_txn* txn;

  if (mdb_txn_begin(mdb_env_, NULL, 0, &txn) != 0) {
    RAISE(kRuntimeError, "mdb_txn_begin() failed");
  }

  if (mdb_dbi_open(txn, NULL, 0, &mdb_handle_) != 0) {
    RAISE(kRuntimeError, "mdb_dbi_open() failed");
  }

  auto rc = mdb_txn_commit(txn);
  if (rc != 0) {
    auto err = String(mdb_strerror(rc));
    RAISEF(kRuntimeError, "mdb_txn_commit() failed: $0", err);
  }
}

}
}
