/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/comm/rpc.h"

namespace fnord {
namespace comm {

AnyRPC::AnyRPC(
    const std::string& method) :
    method_(method),
    ready_(false),
    on_ready_cb_(nullptr) {}

AnyRPC::~AnyRPC() {}

const std::string& AnyRPC::method() const {
  return method_;
};

void AnyRPC::wait() {
  std::unique_lock<std::mutex> l(mutex_);

  while (!ready_) {
    cond_.wait(l);
  }
}

void AnyRPC::ready() {
  iputs("ready...", 1);
  {
    std::unique_lock<std::mutex> l(mutex_);
    ready_ = true;
  }

  if (on_ready_cb_) {
    on_ready_cb_();
  }

  cond_.notify_all();
}

void AnyRPC::onReady(std::function<void ()> on_ready_cb) {
  std::unique_lock<std::mutex> l(mutex_);

  if (ready_) {
    on_ready_cb();
  } else {
    on_ready_cb_ = on_ready_cb;
  }
}

}
}
