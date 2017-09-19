/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <map>
#include <string>
#include <zdb/zdb.h>

zdb_error_t zdb_open(zdb_t* db, const char* filename) {
  return static_cast<zdb::zdb*>(db)->open(filename);
}

namespace zdb {

zdb::zdb() {
  error[0] = '\0';
}

} // namespace zdb

