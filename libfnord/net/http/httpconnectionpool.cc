/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/exception.h"
#include "fnord/net/http/httpconnectionpool.h"

namespace fnord {
namespace http {

HTTPConnectionPool::HTTPConnectionPool(
    fnord::thread::TaskScheduler* scheduler) :
    scheduler_(scheduler) {}

std::unique_ptr<HTTPResponseFuture> HTTPConnectionPool::executeRequest(
    const HTTPRequest& req) {
  if (!req.hasHeader("Host")) {
    RAISE(kRuntimeError, "missing Host header");
  }

  auto addr = fnord::net::InetAddr::resolve(req.getHeader("Host"));
  if (!addr.hasPort()) {
    addr.setPort(80);
  }

  return executeRequest(req, addr);
}

std::unique_ptr<HTTPResponseFuture> HTTPConnectionPool::executeRequest(
    const HTTPRequest& req,
    const fnord::net::InetAddr& addr) {
  std::unique_ptr<HTTPClientConnection> conn;

  conn.reset(
      new HTTPClientConnection(
          std::move(fnord::net::TCPConnection::connect(addr)),
          scheduler_));

  //auto owned = new OwnedConnection();
  //owned->conn = std::move(conn);
  //owned->is_busy = true;

  //{
  //  std::unique_lock<std::mutex> l(connection_cache_mutex_);
  //  connection_cache_.emplace(
  //     addr.ipAndPort(),
  //     std::unique_ptr<OwnedConnection>(owned));
  //}

  std::unique_ptr<HTTPResponseFuture> future(new HTTPResponseFuture());
  conn->executeRequest(req, future->responseHandler());
  future->storeConnection(std::move(conn));

  return future;
}


}
}
