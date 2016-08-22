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
#include <assert.h>
#include "fnordmetric/util/exception.h"
#include "fnordmetric/transport/http/httpresponsefuture.h"

namespace fnordmetric {
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

StreamingResponseHandler::FactoryFn StreamingResponseHandler::getFactory(
    CallbackFn on_event) {
  return [on_event] (
      const Promise<HTTPResponse> promise) -> HTTPResponseFuture* {
    return new StreamingResponseHandler(promise, on_event);
  };
}

StreamingResponseHandler::StreamingResponseHandler(
    Promise<HTTPResponse> promise,
    CallbackFn callback) :
    HTTPResponseFuture(promise),
    callback_(callback) {}

void StreamingResponseHandler::onBodyChunk(
    const char* data,
    size_t size) {
  callback_(data, size);
}

}
}
