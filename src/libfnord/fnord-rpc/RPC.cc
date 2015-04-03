/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-rpc/RPC.h"

namespace fnord {

AnyRPC::AnyRPC(
    const std::string& method) :
    method_(method),
    status_(eSuccess),
    is_ready_(false),
    autodelete_(false) {
  incRef();
}

AnyRPC::~AnyRPC() {}

const Buffer& AnyRPC::encoded() {
  if (encoded_request_.size() == 0) {
    RAISE(kRPCError, "RPC request must be encoded first");
  }

  return encoded_request_;
}

const std::string& AnyRPC::method() const {
  return method_;
};

void AnyRPC::wait() {
  ready_wakeup_.waitForWakeup(0);
  status_.raiseIfError();
}

void AnyRPC::onReady(std::function<void()> callback) {
  ready_wakeup_.onWakeup(0, callback);
}

void AnyRPC::ready(const Buffer& result) noexcept {
  try {
    decode_fn_(result);
  } catch (const std::exception& e) {
    error(e);
  }
}

void AnyRPC::ready() noexcept {
  std::unique_lock<std::mutex> lk(mutex_);
  is_ready_ = true;
  lk.unlock();
  ready_wakeup_.wakeup();
  decRef();
}

void AnyRPC::raiseIfError() const {
  status_.raiseIfError();
}

void AnyRPC::error(const std::exception& e) {
  status_ = Status(e);
  ready();
}

void AnyRPC::error(const Status& status) {
  status_ = status;
  ready();
}

bool AnyRPC::isSuccess() const {
  return status_.isSuccess();
}

bool AnyRPC::isFailure() const {
  return status_.isError();
}

const Status& AnyRPC::status() const {
  return status_;
}

}
