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


}
}
