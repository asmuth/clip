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
  auto conn = leaseConnection(addr);
  std::unique_ptr<HTTPResponseFuture> future(new HTTPResponseFuture());

  try {
    conn->executeRequest(req, future->responseHandler());
  } catch (const std::exception& e) {
    conn->onReady()->wakeup();
  }

  return future;
}

void HTTPConnectionPool::parkConnection(
    HTTPClientConnection* conn,
    net::InetAddr addr) {
  if (conn->isIdle()) {
    std::unique_lock<std::mutex> l(connection_cache_mutex_);
    connection_cache_.emplace(addr.ipAndPort(), conn);
  } else {
    delete conn;
  }
}

HTTPClientConnection* HTTPConnectionPool::leaseConnection(
    const fnord::net::InetAddr& addr) {
  std::unique_ptr<HTTPClientConnection> conn(nullptr);

  {
    std::unique_lock<std::mutex> l(connection_cache_mutex_);
    auto iter = connection_cache_.find(addr.ipAndPort());

    if (iter != connection_cache_.end()) {
      conn.reset(iter->second);
      connection_cache_.erase(iter);
    }
  }

  if (conn.get() == nullptr) {
    conn.reset(
        new HTTPClientConnection(
            std::move(fnord::net::TCPConnection::connect(addr)),
            scheduler_));
  }

  scheduler_->runOnNextWakeup(
      std::bind(&HTTPConnectionPool::parkConnection, this, conn.get(), addr),
      conn->onReady());

  return conn.release();
}

}
}
