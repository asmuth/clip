/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/inspect.h"
#include "fnord/thread/wakeup.h"

namespace fnord {
namespace thread {

Wakeup::Wakeup() : gen_(0) {
  fnord::iputs("wakeup create $0 $1", this, gen_.load());
}

Wakeup::~Wakeup() {
  fnord::iputs("wakeup destroy $0 $1", this, gen_.load());
}

void Wakeup::waitForNextWakeup() {
  waitForWakeup(generation());
}

void Wakeup::waitForWakeup(long oldgen) {

  fnord::iputs("wakeup wait $0 $1", this, oldgen);
  std::unique_lock<std::mutex> l(mutex_);

  while (gen_.load() == oldgen) {
    condvar_.wait(l);
  }
}

long Wakeup::generation() const {
  return gen_.load();
}

void Wakeup::wakeup() {
  {
    std::unique_lock<std::mutex> l(mutex_);
    gen_++;
  }

  condvar_.notify_all();
}

}
}
