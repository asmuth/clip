/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_MDBCURSOR_H
#define _FNORD_MDBCURSOR_H
#include <memory>
#include <vector>
#include <liblmdb/lmdb.h>
#include "fnord-base/autoref.h"
#include "fnord-base/logging.h"
#include "fnord-base/option.h"

namespace fnord {
namespace mdb {

class MDBCursor : public RefCounted {
public:

  MDBCursor(MDB_cursor* mdb_cur);
  MDBCursor(const MDBCursor& other) = delete;
  MDBCursor& operator=(const MDBCursor& other) = delete;

  bool getFirst(Buffer* key, Buffer* value);
  bool getFirst(
      void** key,
      size_t* key_size,
      void** value,
      size_t* value_size);

  bool getLast(Buffer* key, Buffer* value);
  bool getLast(
      void** key,
      size_t* key_size,
      void** value,
      size_t* value_size);

  bool getNext(Buffer* key, Buffer* value);
  bool getNext(
      void** key,
      size_t* key_size,
      void** value,
      size_t* value_size);

  void close();

protected:
  MDB_cursor* mdb_cur_;
  bool is_closed_;
};

}
}
#endif
