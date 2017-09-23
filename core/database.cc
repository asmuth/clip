/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "database.h"
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace zdb {

database::database(
    metadata&& meta_,
    bool readonly_) :
    meta(std::move(meta_)),
    readonly(readonly_) {
  if (pthread_rwlock_init(&lock, nullptr)) {
    throw new std::runtime_error("pthread_rwlock_init failed");
  }
}

database::~database() {
  pthread_rwlock_destroy(&lock);
}

zdb_err_t open(const std::string& filename, int oflags, database_ref* db_ref) {
  auto db = std::make_shared<database>(metadata{}, false);
  *db_ref = std::move(db);
  return ZDB_SUCCESS;
}

} // namespace zdb

