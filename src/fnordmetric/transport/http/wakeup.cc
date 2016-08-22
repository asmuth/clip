/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include "fnordmetric/util/inspect.h"
#include "fnordmetric/transport/http/wakeup.h"

Wakeup::Wakeup() : gen_(0) {
}

void Wakeup::waitForNextWakeup() {
  waitForWakeup(generation());
}

void Wakeup::waitForFirstWakeup() {
  waitForWakeup(0);
}

void Wakeup::waitForWakeup(long oldgen) {
  std::unique_lock<std::mutex> l(mutex_);

  condvar_.wait(l, [this, oldgen] {
    return gen_.load() > oldgen;
  });
}

void Wakeup::onWakeup(long generation, std::function<void()> callback) {
  std::unique_lock<std::mutex> l(mutex_);

  if (gen_.load() > generation) {
    l.unlock();
    callback();
    return;
  }

  callbacks_.push_back(callback);
}

long Wakeup::generation() const {
  return gen_.load();
}

void Wakeup::wakeup() {
  std::list<std::function<void ()>> callbacks;

  std::unique_lock<std::mutex> l(mutex_);
  gen_++;
  callbacks.splice(callbacks.begin(), callbacks_);
  l.unlock();

  condvar_.notify_all();

  for (const auto& callback: callbacks) {
    callback();
  }
}

