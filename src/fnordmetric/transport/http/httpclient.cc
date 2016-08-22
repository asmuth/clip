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
#include "fnordmetric/util/exception.h"
#include "fnordmetric/transport/http/httpclient.h"
#include "fnordmetric/transport/http/httpclientconnection.h"
#include "fnordmetric/util/net/tcpconnection.h"

namespace fnordmetric {
namespace http {

HTTPClient::HTTPClient() : stats_(&stats_int_) {}
HTTPClient::HTTPClient(HTTPClientStats* stats) : stats_(stats) {}

HTTPResponse HTTPClient::executeRequest(
    const HTTPRequest& req) {
  return executeRequest(
      req,
      [] (Promise<HTTPResponse> promise) {
        return new HTTPResponseFuture(promise);
      });
}

Status HTTPClient::executeRequest(
    const HTTPRequest& req,
    HTTPResponse* res) {
  try {
    *res = executeRequest(req);
  } catch (const std::exception& e) {
    return Status(eIOError, e.what());
  }

  return Status::success();
}

HTTPResponse HTTPClient::executeRequest(
    const HTTPRequest& req,
    const InetAddr& addr) {
  return executeRequest(
      req,
      addr,
      [] (Promise<HTTPResponse> promise) {
        return new HTTPResponseFuture(promise);
      });
}

HTTPResponse HTTPClient::executeRequest(
    const HTTPRequest& req,
    Function<HTTPResponseFuture* (Promise<HTTPResponse> promise)> factory) {
  if (!req.hasHeader("Host")) {
    RAISE(kRuntimeError, "missing Host header");
  }

  auto addr = dns_cache_.resolve(req.getHeader("Host"));
  if (!addr.hasPort()) {
    addr.setPort(80);
  }

  return executeRequest(req, addr, factory);
}

HTTPResponse HTTPClient::executeRequest(
    const HTTPRequest& req,
    const InetAddr& addr,
    Function<HTTPResponseFuture* (Promise<HTTPResponse> promise)> factory) {
  std::unique_lock<std::mutex> lk(mutex_);

  Promise<HTTPResponse> promise;
  auto http_future = factory(promise);
  bool failed = false;

  try {
    ScopedPtr<HTTPClientConnection> conn(
        new HTTPClientConnection(
            net::TCPConnection::connect(addr),
            &ev_,
            stats_));

    conn->executeRequest(req, http_future);
    http_future->storeConnection(std::move(conn));
  } catch (const std::exception& e) {
    http_future->onError(e);
    failed = true;
  }

  auto future = promise.future();

  if (!failed) {
    ev_.runOnce();
  }

  return future.waitAndGet();
}

}
}

