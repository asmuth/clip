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
#include "fnord/net/http/httpserverconnection.h"
#include "fnord/net/http/httpgenerator.h"

namespace fnord {

template <>
std::string inspect(const http::HTTPServerConnection& conn) {
  return StringUtil::format("<HTTPServerConnection $0>", inspect(&conn));
}

namespace http {

void HTTPServerConnection::start(
    HTTPHandlerFactory* handler_factory,
    std::unique_ptr<net::TCPConnection> conn,
    thread::TaskScheduler* scheduler) {
  // N.B. we don't leak the connection here. it is ref counted and will
  // free itself
  auto http_conn = new HTTPServerConnection(
      handler_factory,
      std::move(conn),
      scheduler);

  http_conn->nextRequest();
}

HTTPServerConnection::HTTPServerConnection(
    HTTPHandlerFactory* handler_factory,
    std::unique_ptr<net::TCPConnection> conn,
    thread::TaskScheduler* scheduler) :
    handler_factory_(handler_factory),
    conn_(std::move(conn)),
    scheduler_(scheduler),
    on_write_completed_cb_(nullptr),
    refcount_(1) {
  log::Logger::get()->logf(
      fnord::log::kTrace, "New HTTP connection: $0",
      inspect(*this));

  conn_->setNonblocking(true);
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

  parser_.onHeadersComplete(std::bind(&HTTPServerConnection::dispatchRequest, this));
}

void HTTPServerConnection::read() {
  mutex_.lock();

  size_t len;
  try {
    len = conn_->read(buf_.data(), buf_.allocSize());
  } catch (Exception& e) {
    mutex_.unlock();
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
      mutex_.unlock();
      close();
      return;
    } else {
      parser_.parse((char *) buf_.data(), len);
    }
  } catch (Exception& e) {
    mutex_.unlock();

    log::Logger::get()->logException(
        fnord::log::kDebug,
        "HTTP parse error, closing connection",
        e);

    close();
    return;
  }

  if (parser_.state() != HTTPParser::S_DONE) {
    awaitRead();
  }

  mutex_.unlock();
}

void HTTPServerConnection::write() {
  mutex_.lock();

  auto data = ((char *) buf_.data()) + buf_.mark();
  auto size = buf_.size() - buf_.mark();

  size_t len;
  try {
    len = conn_->write(data, size);
    buf_.setMark(buf_.mark() + len);
  } catch (Exception& e) {
    mutex_.unlock();

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
    mutex_.unlock();
    awaitWrite();
  } else {
    buf_.clear();
    mutex_.unlock();
    if (on_write_completed_cb_) {
      on_write_completed_cb_();
    }
  }
}

void HTTPServerConnection::awaitRead() {
  scheduler_->runOnReadable(
      std::bind(&HTTPServerConnection::read, this),
      *conn_);
}

void HTTPServerConnection::awaitWrite() {
  scheduler_->runOnWritable(
      std::bind(&HTTPServerConnection::write, this),
      *conn_);
}

void HTTPServerConnection::nextRequest() {
  parser_.reset();
  cur_request_.reset(new HTTPRequest());
  cur_handler_.reset(nullptr);
  on_write_completed_cb_ = nullptr;
  body_buf_.clear();

  parser_.onBodyChunk([this] (const char* data, size_t size) {
    body_buf_.append(data, size);
  });

  awaitRead();
}

void HTTPServerConnection::dispatchRequest() {
  incRef();
  cur_handler_= handler_factory_->getHandler(this, cur_request_.get());
  cur_handler_->handleHTTPRequest();
}

void HTTPServerConnection::readRequestBody(
    std::function<void (const void*, size_t, bool)> callback) {
  std::lock_guard<std::recursive_mutex> lock_holder(mutex_);

  switch (parser_.state()) {
    case HTTPParser::S_METHOD:
    case HTTPParser::S_URI:
    case HTTPParser::S_VERSION:
    case HTTPParser::S_HEADER:
      RAISE(kIllegalStateError, "can't read body before headers are parsed");
    case HTTPParser::S_BODY:
    case HTTPParser::S_DONE:
      break;
      break;
  }

  auto read_body_chunk_fn = [this, callback] (const char* data, size_t size) {
    auto last_chunk = parser_.state() == HTTPParser::S_DONE;

    if (last_chunk || size > 0) {
      callback(data, size, last_chunk);
    }
  };

  read_body_chunk_fn((const char*) body_buf_.data(), body_buf_.size());
  parser_.onBodyChunk(read_body_chunk_fn);
}

void HTTPServerConnection::discardRequestBody(std::function<void ()> callback) {
  readRequestBody([callback] (const void* data, size_t size, bool last) {
    if (last) {
      callback();
    }
  });
}

void HTTPServerConnection::writeResponse(
    const HTTPResponse& resp,
    std::function<void()> ready_callback) {
  std::lock_guard<std::recursive_mutex> lock_holder(mutex_);

  buf_.clear();
  io::BufferOutputStream os(&buf_);
  HTTPGenerator::generate(resp, &os);
  on_write_completed_cb_ = ready_callback;
  awaitWrite();
}

void HTTPServerConnection::writeResponseBody(
    const void* data,
    size_t size,
    std::function<void()> ready_callback) {
  std::lock_guard<std::recursive_mutex> lock_holder(mutex_);

  buf_.clear();
  buf_.append(data, size);
  on_write_completed_cb_ = ready_callback;
  awaitWrite();
}

void HTTPServerConnection::finishResponse() {
  if (decRef()) {
    return;
  }

  if (cur_request_->keepalive()) {
    std::lock_guard<std::recursive_mutex> lock_holder(mutex_);
    nextRequest();
  } else {
    close();
  }
}

void HTTPServerConnection::close() {
  log::Logger::get()->logf(
      fnord::log::kTrace, "HTTP connection close: $0",
      inspect(*this));

  conn_->close();
  decRef();
}

void HTTPServerConnection::incRef() {
  refcount_++;
}

bool HTTPServerConnection::decRef() {
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

