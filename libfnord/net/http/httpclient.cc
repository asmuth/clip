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

std::unique_ptr<HTTPResponseFuture> HTTPClient::get(
    const std::string& uri,
    fnord::thread::TaskScheduler* sched) {
  return get(URI(uri), HTTPMessage::HeaderList{}, sched);
}

std::unique_ptr<HTTPResponseFuture> HTTPClient::get(
    const std::string& uri,
    const HTTPMessage::HeaderList& headers,
    fnord::thread::TaskScheduler* sched) {
  return get(URI(uri), headers, sched);
}

std::unique_ptr<HTTPResponseFuture> HTTPClient::get(
    const URI& uri,
    fnord::thread::TaskScheduler* sched) {
  return get(uri, HTTPMessage::HeaderList{}, sched);
}

std::unique_ptr<HTTPResponseFuture> HTTPClient::get(
    const URI& uri,
    const HTTPMessage::HeaderList& headers,
    fnord::thread::TaskScheduler* sched) {
  HTTPRequest req(fnord::http::HTTPMessage::M_GET, uri.pathAndQuery());
  req.addHeader("Host", uri.hostAndPort());

  for (const auto& hdr : headers) {
    req.setHeader(hdr.first, hdr.second);
  }

  return executeRequest(req, sched);
}

std::unique_ptr<HTTPResponseFuture> HTTPClient::executeRequest(
    const HTTPRequest& req,
    fnord::thread::TaskScheduler* sched) {
  if (!req.hasHeader("Host")) {
    RAISE(kRuntimeError, "missing Host header");
  }

  auto addr = fnord::net::InetAddr::resolve(req.getHeader("Host"));
  return executeRequest(req, addr, sched);
}

std::unique_ptr<HTTPResponseFuture> HTTPClient::executeRequest(
    const HTTPRequest& req,
    const fnord::net::InetAddr& addr,
    fnord::thread::TaskScheduler* sched) {
  auto conn = std::unique_ptr<HTTPClientConnection>(
      new HTTPClientConnection(
          std::move(fnord::net::TCPConnection::connect(addr)),
          sched));

  std::unique_ptr<HTTPResponseFuture> future(new HTTPResponseFuture());
  conn->executeRequest(req, future->responseHandler());
  future->storeConnection(std::move(conn));

  return future;
}

}
}
