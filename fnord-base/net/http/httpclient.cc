/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/exception.h"
#include "fnord/net/http/httpclient.h"
#include "fnord/net/http/httpclientconnection.h"
#include "fnord/net/tcpconnection.h"

namespace fnord {
namespace http {

Future<HTTPResponse> HTTPClient::get(
    const std::string& uri,
    fnord::TaskScheduler* sched) {
  return executeRequest(HTTPRequest::mkGet(uri), sched);
}

Future<HTTPResponse> HTTPClient::get(
    const std::string& uri,
    const HTTPMessage::HeaderList& headers,
    fnord::TaskScheduler* sched) {
  return executeRequest(HTTPRequest::mkGet(uri, headers), sched);
}

Future<HTTPResponse> HTTPClient::get(
    const URI& uri,
    fnord::TaskScheduler* sched) {
  return executeRequest(HTTPRequest::mkGet(uri), sched);
}

Future<HTTPResponse> HTTPClient::get(
    const URI& uri,
    const HTTPMessage::HeaderList& headers,
    fnord::TaskScheduler* sched) {
  return executeRequest(HTTPRequest::mkGet(uri, headers), sched);
}

Future<HTTPResponse> HTTPClient::executeRequest(
    const HTTPRequest& req,
    fnord::TaskScheduler* sched) {
  if (!req.hasHeader("Host")) {
    RAISE(kRuntimeError, "missing Host header");
  }

  auto addr = fnord::net::InetAddr::resolve(req.getHeader("Host"));
  if (!addr.hasPort()) {
    addr.setPort(80);
  }

  return executeRequest(req, addr, sched);
}

Future<HTTPResponse> HTTPClient::executeRequest(
    const HTTPRequest& req,
    const fnord::net::InetAddr& addr,
    fnord::TaskScheduler* sched) {
  auto conn = std::unique_ptr<HTTPClientConnection>(
      new HTTPClientConnection(
          std::move(fnord::net::TCPConnection::connect(addr)),
          sched,
          nullptr));

  Promise<HTTPResponse> promise;
  auto http_future = new HTTPResponseFuture(promise);

  try {
    conn->executeRequest(req, http_future);
    http_future->storeConnection(std::move(conn));
  } catch (const std::exception& e) {
    http_future->onError(e);
  }

  return promise.future();
}

}
}
