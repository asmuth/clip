/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include "database.h"

namespace zdb {

zdb::database() {
  if (pthread_rwlock_init(lock, nullptr)) {
    throw new std::runtime_error("pthread_rwlock_init failed");
  }
}

zdb::~database() {
  pthread_rwlock_destroy(lock);
}

} // namespace zdb

