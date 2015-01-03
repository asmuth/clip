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
  std::unique_ptr<HTTPResponseFuture> future(new HTTPResponseFuture());
  auto f = future.get();

  leaseConnection(addr, f, [this, req, f] (HTTPClientConnection* conn) {
    try {
      conn->executeRequest(req, f->responseHandler());
    } catch (const std::exception& e) {
      conn->onReady()->wakeup();
      f->responseHandler()->onError(e);
    }
  });

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

void HTTPConnectionPool::leaseConnection(
    const fnord::net::InetAddr& addr,
    HTTPResponseFuture* future,
    std::function<void (HTTPClientConnection* conn)> callback) {
  std::unique_lock<std::mutex> lk(connection_cache_mutex_);
  auto iter = connection_cache_.find(addr.ipAndPort());

  if (iter != connection_cache_.end()) {
    std::unique_ptr<HTTPClientConnection> conn(iter->second);
    connection_cache_.erase(iter);
    lk.unlock();

    scheduler_->runOnNextWakeup(
        std::bind(&HTTPConnectionPool::parkConnection, this, conn.get(), addr),
        conn->onReady());

    callback(conn.release());
  } else {
    lk.unlock();

    try {
      net::TCPConnection::connectAsync(
          addr,
          scheduler_,
          [this, future, callback, addr] (
              std::unique_ptr<net::TCPConnection> tcp_conn) {
            try {
              tcp_conn->checkErrors();

              std::unique_ptr<HTTPClientConnection> conn(
                  new HTTPClientConnection(std::move(tcp_conn), scheduler_));

              scheduler_->runOnNextWakeup(
                  std::bind(
                      &HTTPConnectionPool::parkConnection,
                      this,
                      conn.get(),
                      addr),
                  conn->onReady());

              callback(conn.release());
            } catch (const std::exception& e) {
              future->responseHandler()->onError(e);
            }
          });
    } catch (const std::exception& e) {
      future->responseHandler()->onError(e);
    }
  }
}

}
}
