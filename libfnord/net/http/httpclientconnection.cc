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

void HTTPClientConnection::executeRequest(const HTTPRequest& req) {
  std::unique_lock<std::recursive_mutex> l(mutex_);

  if (state_ != S_CONN_IDLE) {
    RAISE(
        kIllegalStateError,
        "executeRequest called on non-idle HTTP connection");
  }

  buf_.clear();
  io::BufferOutputStream os(&buf_);
  HTTPGenerator::generate(req, &os);
  state_ = S_CONN_BUSY;

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
  std::unique_lock<std::recursive_mutex> l(mutex_);

  size_t len;
  try {
    len = conn_->read(buf_.data(), buf_.allocSize());
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      return awaitRead();
    }

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

  if (parser_.state() == HTTPParser::S_DONE) {
    fnord::iputs("response received!!", 1);
    close(); // FIXPAUL keepalive
  } else {
    awaitRead();
  }
}

void HTTPClientConnection::write() {
  std::unique_lock<std::recursive_mutex> l(mutex_);

  auto data = ((char *) buf_.data()) + buf_.mark();
  auto size = buf_.size() - buf_.mark();

  size_t len;
  try {
    len = conn_->write(data, size);
    buf_.setMark(buf_.mark() + len);
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      return awaitWrite();
    }

    log::Logger::get()->logException(
        fnord::log::kDebug,
        "HTTP write() failed, closing connection",
        e);

    close();
    return;
  }

  if (buf_.mark() < buf_.size()) {
    awaitWrite();
  } else {
    buf_.clear();
    awaitRead();
  }
}

}
}
