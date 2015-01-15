/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_MDBTRANSACTION_H
#define _FNORD_MDBTRANSACTION_H
#include <memory>
#include <vector>
#include <liblmdb/lmdb.h>
#include "fnord/base/autoref.h"
#include "fnord/base/logging.h"
#include "fnord/base/option.h"

namespace fnord {
namespace mdb {

class MDBTransaction : public RefCounted {
public:

  MDBTransaction(MDB_txn* mdb_txn, MDB_dbi mdb_handle);
  ~MDBTransaction();
  MDBTransaction(const MDBTransaction& other) = delete;
  MDBTransaction& operator=(const MDBTransaction& other) = delete;

  void commit();
  void abort();

  Option<Buffer> get(const Buffer& key);
  Option<Buffer> get(const String& key);
  bool get(const void* key, size_t key_size, void** data, size_t* data_size);

protected:
  MDB_txn* mdb_txn_;
  MDB_dbi mdb_handle_;
  bool is_commited_;
};

}
}
#endif
