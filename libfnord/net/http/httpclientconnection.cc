/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/exception.h>
#include <fnord/base/inspect.h>
#include <fnord/net/http/httpgenerator.h>
#include <fnord/net/http/httpclientconnection.h>
#include <fnord/net/http/httpresponsehandler.h>

namespace fnord {
namespace http {

HTTPClientConnection::HTTPClientConnection(
    std::unique_ptr<net::TCPConnection> conn,
    thread::TaskScheduler* scheduler) :
    conn_(std::move(conn)),
    scheduler_(scheduler),
    state_(S_CONN_IDLE),
    parser_(HTTPParser::PARSE_HTTP_RESPONSE) {
  buf_.reserve(kMinBufferSize);
  conn_->checkErrors();
}

HTTPClientConnection::~HTTPClientConnection() {
  if (state_ != S_CONN_CLOSED) {
    close();
  }
}

void HTTPClientConnection::executeRequest(
    const HTTPRequest& request,
    HTTPResponseHandler* response_handler) {
  std::unique_lock<std::recursive_mutex> l(mutex_);

  if (state_ != S_CONN_IDLE) {
    RAISE(
        kIllegalStateError,
        "executeRequest called on non-idle HTTP connection");
  }

  buf_.clear();
  io::BufferOutputStream os(&buf_);
  HTTPGenerator::generate(request, &os);
  state_ = S_CONN_BUSY;
  cur_handler_ = response_handler;

  awaitWrite();
}

void HTTPClientConnection::awaitRead() {
  scheduler_->runOnReadable(
      std::bind(&HTTPClientConnection::read, this),
      *conn_);
}

void HTTPClientConnection::awaitWrite() {
  scheduler_->runOnWritable(
      std::bind(&HTTPClientConnection::write, this),
      *conn_);
}

void HTTPClientConnection::close() {
  state_ = S_CONN_CLOSED;
}

void HTTPClientConnection::read() {
  mutex_.lock();

  size_t len;
  try {
    len = conn_->read(buf_.data(), buf_.allocSize());
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      std::lock_guard<std::recursive_mutex> l(mutex_, std::adopt_lock_t {});
      return awaitRead();
    } else {
      close();
      mutex_.unlock();
      cur_handler_->onError(e);
    }
    return;
  }

  try {
    if (len == 0) {
      parser_.eof();
    } else {
      parser_.parse((char *) buf_.data(), len);
    }
  } catch (Exception& e) {
    close();
    mutex_.unlock();
    cur_handler_->onError(e);
    return;
  }

  if (parser_.state() == HTTPParser::S_DONE) {
    close(); // FIXPAUL keepalive
    mutex_.unlock();
    cur_handler_->onResponseComplete();
  } else {
    std::lock_guard<std::recursive_mutex> l(mutex_, std::adopt_lock_t {});
    awaitRead();
  }
}

void HTTPClientConnection::write() {
  mutex_.lock();

  auto data = ((char *) buf_.data()) + buf_.mark();
  auto size = buf_.size() - buf_.mark();

  size_t len;
  try {
    len = conn_->write(data, size);
    buf_.setMark(buf_.mark() + len);
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      std::lock_guard<std::recursive_mutex> l(mutex_, std::adopt_lock_t {});
      return awaitWrite();
    } else {
      close();
      mutex_.unlock();
      cur_handler_->onError(e);
      return;
    }
  }

  std::lock_guard<std::recursive_mutex> l(mutex_, std::adopt_lock_t {});

  if (buf_.mark() < buf_.size()) {
    awaitWrite();
  } else {
    buf_.clear();
    awaitRead();
  }
}

}
}
