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
#include "fnord/base/logging.h"
#include "fnord/base/autoref.h"

namespace fnord {
namespace mdb {

class MDBTransaction : public RefCounted {
public:

  MDBTransaction(MDB_txn* mdb_txn);
  ~MDBTransaction();
  MDBTransaction(const MDBTransaction& other) = delete;
  MDBTransaction& operator=(const MDBTransaction& other) = delete;

  void commit();
  void abort();

protected:
  MDB_txn* mdb_txn_;
  bool is_commited_;
};

}
}
#endif
