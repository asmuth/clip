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
    TaskScheduler* scheduler,
    HTTPClientStats* stats) :
    conn_(std::move(conn)),
    scheduler_(scheduler),
    stats_(stats),
    state_(S_CONN_IDLE),
    parser_(HTTPParser::PARSE_HTTP_RESPONSE),
    keepalive_(false) {
  buf_.reserve(kMinBufferSize);
  conn_->checkErrors();

  if (stats_) {
    stats_->current_connections.incr(1);
    stats_->total_connections.incr(1);
  }
}

HTTPClientConnection::~HTTPClientConnection() {
  if (stats_) {
    stats_->current_connections.decr(1);
  }

  if (state_ != S_CONN_CLOSED) {
    close();
  }
}

Wakeup* HTTPClientConnection::onReady() {
  return &on_ready_;
}

bool HTTPClientConnection::isIdle() const {
  return state_ == S_CONN_IDLE;
}

void HTTPClientConnection::executeRequest(
    const HTTPRequest& request,
    HTTPResponseHandler* response_handler) {
  std::unique_lock<std::mutex> l(mutex_);

  if (state_ != S_CONN_IDLE) {
    RAISE(
        kIllegalStateError,
        "executeRequest called on non-idle HTTP connection");
  }

  parser_.onVersion([this] (const char* data, size_t size) {
    cur_handler_->onVersion(std::string(data, size));
  });

  parser_.onStatusName([this] (const char* data, size_t size) {
    cur_handler_->onStatusName(std::string(data, size));
  });

  parser_.onStatusCode([this] (int code) {
    cur_handler_->onStatusCode(code);
  });

  parser_.onHeader([this] (
      const char* key,
      size_t key_size,
      const char* val,
      size_t val_size) {
    cur_handler_->onHeader(
        std::string(key, key_size),
        std::string(val, val_size));
  });


  parser_.onHeadersComplete([this] () {
    cur_handler_->onHeadersComplete();
  });

  parser_.onBodyChunk([this] (const char* data, size_t size) {
    cur_handler_->onBodyChunk(data, size);
  });

  buf_.clear();
  BufferOutputStream os(&buf_);
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
  conn_->close();
}

void HTTPClientConnection::keepalive() {
  state_ = S_CONN_IDLE;
  parser_.reset();
  buf_.clear();
  keepalive_ = false;
}

void HTTPClientConnection::read() {
  mutex_.lock();

  size_t len;
  try {
    len = conn_->read(buf_.data(), buf_.allocSize());
    if (stats_) {
      stats_->received_bytes.incr(len);
    }
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      std::lock_guard<std::mutex> l(mutex_, std::adopt_lock_t {});
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
    if (keepalive_) {
      keepalive();
    } else {
      close();
    }

    mutex_.unlock();

    scheduler_->runOnNextWakeup(
        std::bind(&HTTPResponseHandler::onResponseComplete, cur_handler_),
        &on_ready_);

    on_ready_.wakeup();
  } else {
    std::lock_guard<std::mutex> l(mutex_, std::adopt_lock_t {});
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
    if (stats_) {
      stats_->sent_bytes.incr(len);
    }
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      std::lock_guard<std::mutex> l(mutex_, std::adopt_lock_t {});
      return awaitWrite();
    } else {
      close();
      mutex_.unlock();
      cur_handler_->onError(e);
      return;
    }
  }

  std::lock_guard<std::mutex> l(mutex_, std::adopt_lock_t {});

  if (buf_.mark() < buf_.size()) {
    awaitWrite();
  } else {
    buf_.clear();
    awaitRead();
  }
}

}
}
