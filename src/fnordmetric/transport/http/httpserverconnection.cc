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
#include "fnordmetric/util/exception.h"
#include "fnordmetric/util/inspect.h"
#include "fnordmetric/util/logging.h"
#include "fnordmetric/transport/http/httpserverconnection.h"
#include "fnordmetric/transport/http/httpgenerator.h"

template <>
std::string inspect(const http::HTTPServerConnection& conn) {
  return StringUtil::format("<HTTPServerConnection $0>", inspect(&conn));
}

namespace fnordmetric {
namespace http {

void HTTPServerConnection::start(
    HTTPHandlerFactory* handler_factory,
    ScopedPtr<TCPConnection> conn,
    TaskScheduler* scheduler,
    HTTPServerStats* stats) {
  auto http_conn = new HTTPServerConnection(
      handler_factory,
      std::move(conn),
      scheduler,
      stats);

  // N.B. we don't leak the connection here. it is ref counted and will
  // free itself
  http_conn->incRef();
  http_conn->nextRequest();
}

HTTPServerConnection::HTTPServerConnection(
    HTTPHandlerFactory* handler_factory,
    ScopedPtr<TCPConnection> conn,
    TaskScheduler* scheduler,
    HTTPServerStats* stats) :
    handler_factory_(handler_factory),
    conn_(std::move(conn)),
    scheduler_(scheduler),
    parser_(HTTPParser::PARSE_HTTP_REQUEST),
    on_write_completed_cb_(nullptr),
    closed_(false),
    stats_(stats) {
  logTrace("http.server", "New HTTP connection: $0", inspect(*this));
  stats_->total_connections.incr(1);
  stats_->current_connections.incr(1);

  conn_->setNonblocking(true);
  read_buf_.reserve(kMinBufferSize);

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

  parser_.onHeadersComplete(
      std::bind(&HTTPServerConnection::dispatchRequest, this));
}

HTTPServerConnection::~HTTPServerConnection() {
  stats_->current_connections.decr(1);
}

void HTTPServerConnection::read() {
  std::unique_lock<std::recursive_mutex> lk(mutex_);

  size_t len;
  try {
    len = conn_->read(read_buf_.data(), read_buf_.allocSize());
    stats_->received_bytes.incr(len);
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      return awaitRead();
    }

    if (on_error_cb_) {
      on_error_cb_();
    }

    lk.unlock();
    logDebug("http.server", e, "read() failed, closing...");

    close();
    return;
  }

  try {
    if (len == 0) {
      parser_.eof();

      if (on_error_cb_) {
        on_error_cb_();
      }

      lk.unlock();
      close();
      return;
    } else {
      parser_.parse((char *) read_buf_.data(), len);
    }
  } catch (Exception& e) {
    logDebug("http.server", e, "HTTP parse error, closing...");

    if (on_error_cb_) {
      on_error_cb_();
    }

    lk.unlock();
    close();
    return;
  }

  if (parser_.state() != HTTPParser::S_DONE) {
    awaitRead();
  }
}

void HTTPServerConnection::write() {
  std::unique_lock<std::recursive_mutex> lk(mutex_);

  auto data = ((char *) write_buf_.data()) + write_buf_.mark();
  auto size = write_buf_.size() - write_buf_.mark();

  size_t len;
  try {
    len = conn_->write(data, size);
    write_buf_.setMark(write_buf_.mark() + len);
    stats_->sent_bytes.incr(len);
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      return awaitWrite();
    }

    logDebug("http.server", e, "write() failed, closing...");
    if (on_error_cb_) {
      on_error_cb_();
    }

    lk.unlock();
    close();
    return;
  }

  if (write_buf_.mark() < write_buf_.size()) {
    awaitWrite();
  } else {
    write_buf_.clear();
    lk.unlock();
    if (on_write_completed_cb_) {
      on_write_completed_cb_();
    }
  }
}

// precondition: must hold mutex
void HTTPServerConnection::awaitRead() {
  if (closed_) {
    RAISE(kIllegalStateError, "read() on closed HTTP connection");
  }

  scheduler_->runOnReadable(
      std::bind(&HTTPServerConnection::read, this),
      *conn_);
}

// precondition: must hold mutex
void HTTPServerConnection::awaitWrite() {
  if (closed_) {
    RAISE(kIllegalStateError, "write() on closed HTTP connection");
  }

  scheduler_->runOnWritable(
      std::bind(&HTTPServerConnection::write, this),
      *conn_);
}

void HTTPServerConnection::nextRequest() {
  parser_.reset();
  cur_request_.reset(new HTTPRequest());
  cur_handler_.reset(nullptr);
  on_write_completed_cb_ = nullptr;
  on_error_cb_ = nullptr;
  body_buf_.clear();

  parser_.onBodyChunk([this] (const char* data, size_t size) {
    std::unique_lock<std::recursive_mutex> lk(mutex_);
    body_buf_.append(data, size);
  });

  awaitRead();
}

void HTTPServerConnection::dispatchRequest() {
  stats_->total_requests.incr(1);
  stats_->current_requests.incr(1);

  cur_handler_= handler_factory_->getHandler(this, cur_request_.get());
  cur_handler_->handleHTTPRequest();
}

void HTTPServerConnection::readRequestBody(
    Function<void (const void*, size_t, bool)> callback,
    Function<void()> on_error) {
  std::unique_lock<std::recursive_mutex> lk(mutex_);

  switch (parser_.state()) {
    case HTTPParser::S_REQ_METHOD:
    case HTTPParser::S_REQ_URI:
    case HTTPParser::S_REQ_VERSION:
    case HTTPParser::S_RES_VERSION:
    case HTTPParser::S_RES_STATUS_CODE:
    case HTTPParser::S_RES_STATUS_NAME:
    case HTTPParser::S_HEADER:
      RAISE(kIllegalStateError, "can't read body before headers are parsed");
    case HTTPParser::S_BODY:
    case HTTPParser::S_DONE:
      break;
  }

  auto read_body_chunk_fn = [this, callback] (const char* data, size_t size) {
    std::unique_lock<std::recursive_mutex> lk(mutex_);
    body_buf_.append(data, size);
    auto last_chunk = parser_.state() == HTTPParser::S_DONE;

    if (last_chunk || body_buf_.size() > 0) {
      BufferRef chunk(new Buffer(body_buf_));

      scheduler_->runAsync([callback, chunk, last_chunk] {
        callback(
            (const char*) chunk->data(),
            chunk->size(),
            last_chunk);
      });

      body_buf_.clear();
    }
  };

  parser_.onBodyChunk(read_body_chunk_fn);
  on_error_cb_ = on_error;
  lk.unlock();
  read_body_chunk_fn(nullptr, 0);
}

void HTTPServerConnection::discardRequestBody(
    Function<void ()> callback,
    Function<void()> on_error) {
  readRequestBody([callback] (const void* data, size_t size, bool last) {
    if (last) {
      callback();
    }
  }, on_error);
}

void HTTPServerConnection::writeResponse(
    const HTTPResponse& resp,
    Function<void()> ready_callback,
    Function<void()> on_error) {
  std::lock_guard<std::recursive_mutex> lk(mutex_);

  if (parser_.state() != HTTPParser::S_DONE) {
    RAISE(kIllegalStateError, "can't write response before request is read");
  }

  BufferOutputStream os(&write_buf_);
  HTTPGenerator::generate(resp, &os);
  on_write_completed_cb_ = ready_callback;
  on_error_cb_ = on_error;
  awaitWrite();
}

void HTTPServerConnection::writeResponseBody(
    const void* data,
    size_t size,
    Function<void()> ready_callback,
    Function<void()> on_error) {
  std::lock_guard<std::recursive_mutex> lk(mutex_);

  if (parser_.state() != HTTPParser::S_DONE) {
    RAISE(kIllegalStateError, "can't write response before request is read");
  }

  write_buf_.append(data, size);
  on_write_completed_cb_ = ready_callback;
  on_error_cb_ = on_error;
  awaitWrite();
}

void HTTPServerConnection::finishResponse() {
  stats_->current_requests.decr(1);

  if (false && cur_request_->keepalive()) {
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    nextRequest();
  } else {
    close();
  }
}

// precondition: must not hold mutex
void HTTPServerConnection::close() {
  std::unique_lock<std::recursive_mutex> lk(mutex_);

  logTrace("http.server", "HTTP connection close: $0", inspect(*this));

  if (closed_) {
    RAISE(kIllegalStateError, "HTTP connection is already closed");
  }

  closed_ = true;
  scheduler_->cancelFD(conn_->fd());
  conn_->close();

  lk.unlock();
  decRef();
}

bool HTTPServerConnection::isClosed() const {
  std::unique_lock<std::recursive_mutex> lk(mutex_);
  return closed_;
}

} // namespace http
} // namespace fnordmetric
