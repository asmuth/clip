/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
 */
#pragma once
#include <stdlib.h>
#include <pthread.h>

namespace zdb {

class lock_guard {
public:
  lock_guard(pthread_rwlock_t* lock);
  lock_guard(const lock_guard& o) = delete;
  ~lock_guard();
  lock_guard& operator=(const lock_guard& o) = delete;

  void lock_write();
  void lock_read();

protected:
  pthread_rwlock_t* lock;
  bool locked;
};

} // namespace zdb

