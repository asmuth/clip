/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include "fnord/base/exception.h"
#include "fnord/net/http/httpresponsefuture.h"

namespace fnord {
namespace http {

HTTPResponseFuture::HTTPResponseFuture() : res_handler_(&res_, &wakeup_) {}

HTTPResponseFuture::~HTTPResponseFuture() {
  assert(wakeup_.generation() == 1); // pending HTTPResponseFuture destroyed
}

const HTTPResponse& HTTPResponseFuture::get() const {
  if (wakeup_.generation() == 0) {
    RAISE(kIllegalStateError, "get() called on pending HTTPResponseFuture");
  }

  return res_;
}

void HTTPResponseFuture::wait() {
  wakeup_.waitForWakeup(0);
}

thread::Wakeup* HTTPResponseFuture::onReady() {
  return &wakeup_;
}

HTTPResponseHandler* HTTPResponseFuture::responseHandler() {
  return &res_handler_;
}

void HTTPResponseFuture::storeConnection(
    std::unique_ptr<HTTPClientConnection>&& conn) {
  conn_ = std::move(conn);
}

}
}
