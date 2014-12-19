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
#include "fnord/base/inspect.h"
#include "fnord/net/http/httpconnection.h"

namespace fnord {

template <>
std::string inspect(const http::HTTPConnection& conn) {
  return "<HTTPConnection>";
}

namespace http {

void HTTPConnection::start(
    std::unique_ptr<net::TCPConnection> conn,
    thread::TaskScheduler* scheduler) {
  // N.B. we don't leak the connection here. it is ref counted and will
  // free itself
  auto http_conn = new HTTPConnection(std::move(conn), scheduler);
  http_conn->readNextRequestHeaders();
}

HTTPConnection::HTTPConnection(
    std::unique_ptr<net::TCPConnection> conn,
    thread::TaskScheduler* scheduler) :
    conn_(std::move(conn)),
    scheduler_(scheduler),
    on_request_cb_(nullptr),
    on_read_completed_cb_(nullptr),
    on_write_completed_cb_(nullptr),
    refcount_(1) {
  log::Logger::get()->logf(
      fnord::log::kTrace, "New HTTP connection: $0",
      inspect(*this));

  buf_.reserve(kMinBufferSize);

  parser_.onMethod([this] (HTTPMessage::kHTTPMethod method) {
    cur_request_->setMethod(method);
  });

  parser_.onURI([this] (const char* data, size_t size) {
    cur_request_->setURI(std::string(data, size));
  });

  parser_.onVersion([this] (const char* data, size_t size) {
    cur_request_->setVersion(std::string(data, size));
  });

  parser_.onHeader([this] (
      const char* key,
      size_t key_size,
      const char* val,
      size_t val_size) {
    cur_request_->addHeader(
        std::string(key, key_size),
        std::string(val, val_size));
  });

  parser_.onHeadersComplete(std::bind(&HTTPConnection::dispatchRequest, this));
}

void HTTPConnection::read() {
  size_t len;
  try {
    len = conn_->read(buf_.data(), buf_.allocSize());
  } catch (Exception& e) {
    log::Logger::get()->logException(
        fnord::log::kDebug,
        "HTTP read() failed, closing connection",
        e);

    close();
    return;
  }

  try {
    if (len == 0) {
      parser_.eof();
      close();
      return;
    } else {
      parser_.parse((char *) buf_.data(), len);
    }
  } catch (Exception& e) {
    log::Logger::get()->logException(
        fnord::log::kDebug,
        "HTTP parse error, closing connection",
        e);

    close();
    return;
  }

  if (on_read_completed_cb_) {
    on_read_completed_cb_();
  }

  if (decRef()) {
    return;
  }
}

void HTTPConnection::write() {
  auto data = ((char *) buf_.data()) + buf_.mark();
  auto size = buf_.size() - buf_.mark();

  size_t len;
  try {
    len = conn_->write(data, size);
  } catch (Exception& e) {
    log::Logger::get()->logException(
        fnord::log::kDebug,
        "HTTP write() failed, closing connection",
        e);

    close();
    return;
  }

  if (buf_.mark() + len < buf_.size()) {
    buf_.setMark(buf_.mark() + len);
    awaitWrite();
  } else {
    buf_.clear();

    if (on_write_completed_cb_) {
      on_write_completed_cb_();
    }
  }

  if (decRef()) {
    return;
  }
}

void HTTPConnection::awaitRead() {
  incRef();

  scheduler_->runOnReadable(
      std::bind(&HTTPConnection::read, this),
      *conn_);
}

void HTTPConnection::awaitWrite() {
  incRef();

  scheduler_->runOnWritable(
      std::bind(&HTTPConnection::write, this),
      *conn_);
}

void HTTPConnection::dispatchRequest() {
  HTTPResponse response;
  response.setStatus(kStatusNotFound);
  response.addBody("Not Found");

  writeResponseHeaders(
      response,
      std::bind(&HTTPConnection::finishResponse, this));
}

void HTTPConnection::readNextRequestHeaders() {
  parser_.reset();
  cur_request_.reset(new HTTPRequest());

  on_write_completed_cb_ = nullptr;
  on_read_completed_cb_ = [this] () {
    if (parser_.state() < HTTPParser::S_BODY) {
      awaitRead();
    }
  };

  awaitRead();
}

void HTTPConnection::writeResponseHeaders(
    const HTTPResponse& resp,
    std::function<void()> ready_callback) {
  std::string res = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
  buf_.clear();
  buf_.append(res.c_str(), res.length());

  on_write_completed_cb_ = ready_callback;
  awaitWrite();
}

void HTTPConnection::writeResponseBody(
    const void* data,
    size_t size,
    std::function<void()> ready_callback) {
}

void HTTPConnection::finishResponse() {
  if (cur_request_->keepalive()) {
    readNextRequestHeaders();
  } else {
    close();
  }
}

void HTTPConnection::close() {
  log::Logger::get()->logf(
      fnord::log::kTrace, "HTTP connection close: $0",
      inspect(*this));

  conn_->close();
  decRef();
}

void HTTPConnection::incRef() {
  refcount_++;
}

bool HTTPConnection::decRef() {
  if (refcount_.fetch_sub(1) == 1) {
    log::Logger::get()->logf(
        fnord::log::kTrace, "HTTP connection free'd: $0",
        inspect(*this));
    delete this;
    return true;
  }

  return false;
}

} // namespace http
} // namespace fnord

