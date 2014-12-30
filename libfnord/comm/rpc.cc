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
#include "fnord/logging/logger.h"

namespace fnord {
namespace comm {

AnyRPC::AnyRPC(
    const std::string& method) :
    method_(method),
    is_error_(false),
    is_ready_(false),
    autodelete_(false) {}

AnyRPC::~AnyRPC() {}

const std::string& AnyRPC::method() const {
  return method_;
};

void AnyRPC::wait() {
  ready_wakeup_.waitForWakeup(0);
}

void AnyRPC::ready() {
  std::unique_lock<std::mutex> lk(mutex_);
  is_ready_ = true;
  ready_wakeup_.wakeup();

  if (autodelete_) {
    lk.unlock();
    reap();
  }
}

void AnyRPC::error(const std::exception& e) {
  is_error_ = true;

  try {
    auto rte = dynamic_cast<const fnord::Exception&>(e);
    error_ = rte.getMessage();
  } catch (const std::exception& cast_error) {
    error_ = e.what();
  }

  ready();
}

void AnyRPC::fireAndForget(std::unique_ptr<AnyRPC>&& rpc) {
  auto rpc_raw = rpc.release();
  rpc_raw->fireAndForget();
}

void AnyRPC::fireAndForget() {
  std::unique_lock<std::mutex> lk(mutex_);

  if (is_ready_) {
    lk.unlock();
    reap();
    return;
  } else {
    autodelete_ = true;
  }
}

void AnyRPC::reap() {
  if (is_error_) {
    fnord::log::Logger::get()->logf(
        log::kWarning,
        "Fire-And-Forget RPC failed: $0",
        error_);
  }

  delete this;
}

}
}
