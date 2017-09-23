/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
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

