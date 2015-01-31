/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/autoref.h"

namespace fnord {

RefCounted::RefCounted() : refcount_(0) {}

void RefCounted::incRef() {
  std::atomic_fetch_add_explicit(&refcount_, 1u, std::memory_order_relaxed);
}

bool RefCounted::decRef() {
  if (std::atomic_fetch_sub_explicit(
          &refcount_,
          1u,
          std::memory_order_release) == 1) {
    std::atomic_thread_fence(std::memory_order_acquire);
    delete this;
    return true;
  }

  return false;
}

} // namespace fnord
