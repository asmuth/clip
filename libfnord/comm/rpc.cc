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
    is_error_(false) {}

AnyRPC::~AnyRPC() {}

const std::string& AnyRPC::method() const {
  return method_;
};

void AnyRPC::wait() {
  ready_wakeup_.waitForWakeup(0);
}

void AnyRPC::ready() {
  ready_wakeup_.wakeup();
}

void AnyRPC::error(const std::exception& e) {
  is_error_ = true;

  try {
    auto rte = dynamic_cast<const fnord::Exception&>(e);
    error_ = rte.getMessage();
  } catch (const std::exception& cast_error) {
    error_ = e.what();
  }

  ready_wakeup_.wakeup();
}

}
}
