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
#include "fnord-base/exception.h"
#include "fnord/net/http/httpresponsefuture.h"

namespace fnord {
namespace http {

HTTPResponseFuture::HTTPResponseFuture(
    Promise<HTTPResponse> promise) :
    promise_(promise) {}

HTTPResponseFuture::~HTTPResponseFuture() {
  assert(promise_.isFulfilled() == true); // pending ResponseFuture destroyed
}

void HTTPResponseFuture::storeConnection(
    std::unique_ptr<HTTPClientConnection>&& conn) {
  conn_ = std::move(conn);
}

void HTTPResponseFuture::onError(const std::exception& e) {
  promise_.failure(e);
  delete this;
}

void HTTPResponseFuture::onVersion(const std::string& version) {
  res_.setVersion(version);
}

void HTTPResponseFuture::onStatusCode(int status_code) {
  res_.setStatusCode(status_code);
}

void HTTPResponseFuture::onStatusName(const std::string& status) {
  res_.setStatusName(status);
}

void HTTPResponseFuture::onHeader(
    const std::string& key,
    const std::string& value) {
  res_.addHeader(key, value);
}

void HTTPResponseFuture::onHeadersComplete() {}

void HTTPResponseFuture::onBodyChunk(const char* data, size_t size) {
  res_.appendBody((char *) data, size);
}

void HTTPResponseFuture::onResponseComplete() {
  promise_.success(res_);
  delete this;
}

}
}
