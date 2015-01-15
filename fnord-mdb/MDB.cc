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
}

MDB::MDB(MDB_env* mdb_env) : mdb_env_(mdb_env) {}

MDB::~MDB() {
  mdb_env_close(mdb_env_);
}

}
}
