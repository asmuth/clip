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
#include <fnordmetric/util/exception.h>
#include <fnordmetric/util/inspect.h>
#include <fnordmetric/transport/http/httpgenerator.h>
#include <fnordmetric/transport/http/httpclientconnection.h>
#include <fnordmetric/transport/http/httpresponsehandler.h>

namespace fnordmetric {
namespace http {

HTTPClientConnection::HTTPClientConnection(
    std::unique_ptr<net::TCPConnection> conn,
    TaskScheduler* scheduler,
    HTTPClientStats* stats) :
    conn_(std::move(conn)),
    scheduler_(scheduler),
    state_(S_CONN_IDLE),
    parser_(HTTPParser::PARSE_HTTP_RESPONSE),
    keepalive_(false),
    stats_(stats) {
  buf_.reserve(kMinBufferSize);
  conn_->checkErrors();

  if (stats_ != nullptr) {
    stats_->current_connections.incr(1);
    stats_->total_connections.incr(1);
  }
}

HTTPClientConnection::~HTTPClientConnection() {
  if (stats_ != nullptr) {
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

  if (request.method() == HTTPRequest::M_HEAD) {
    parser_.ignoreBody();
  }

  buf_.clear();
  BufferOutputStream os(&buf_);
  HTTPGenerator::generate(request, &os);
  state_ = S_CONN_BUSY;
  cur_handler_ = response_handler;

  if (stats_ != nullptr) {
    stats_->current_requests.incr(1);
    stats_->total_requests.incr(1);
  }

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
  if (state_ == S_CONN_CLOSED) {
    RAISE(kIllegalStateError, "connection already closed");
  }

  state_ = S_CONN_CLOSED;
  scheduler_->cancelFD(conn_->fd());
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
    if (stats_ != nullptr) {
      stats_->received_bytes.incr(len);
    }
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      std::lock_guard<std::mutex> l(mutex_, std::adopt_lock_t {});
      return awaitRead();
    } else {
      close();
      mutex_.unlock();
      error(e);
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
    error(e);
    return;
  }

  if (parser_.state() == HTTPParser::S_DONE) {
    if (stats_ != nullptr) {
      stats_->current_requests.decr(1);
    }

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
    if (stats_ != nullptr) {
      stats_->sent_bytes.incr(len);
    }
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      std::lock_guard<std::mutex> l(mutex_, std::adopt_lock_t {});
      return awaitWrite();
    } else {
      close();
      mutex_.unlock();
      error(e);
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


void HTTPClientConnection::error(const std::exception& e) {
  if (cur_handler_) {
    if (stats_ != nullptr) {
      stats_->current_requests.decr(1);
    }
  }

  on_ready_.wakeup();
  cur_handler_->onError(e);
}

}
}

