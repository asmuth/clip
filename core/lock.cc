/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdexcept>
#include "lock.h"

namespace zdb {

lock_guard::lock_guard(pthread_rwlock_t* l) : lock(l), locked(false) {}

lock_guard::~lock_guard() {
  if (locked) {
    pthread_rwlock_unlock(lock);
  }
}

void lock_guard::lock_write() {
  assert(!locked);

  if (pthread_rwlock_wrlock(lock)) {
    throw std::runtime_error("pthread_wrlock_rdlock failed");
  }

  locked = true;
}

void lock_guard::lock_read() {
  assert(!locked);

  if (pthread_rwlock_rdlock(lock)) {
    throw std::runtime_error("pthread_rwlock_rdlock failed");
  }

  locked = true;
}

} // namespace zdb

