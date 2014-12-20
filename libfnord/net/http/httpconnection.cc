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
#include "fnord/net/http/httpgenerator.h"

namespace fnord {

template <>
std::string inspect(const http::HTTPConnection& conn) {
  return "<HTTPConnection>";
}

namespace http {

void HTTPConnection::start(
    HTTPHandlerFactory* handler_factory,
    std::unique_ptr<net::TCPConnection> conn,
    thread::TaskScheduler* scheduler) {
  // N.B. we don't leak the connection here. it is ref counted and will
  // free itself
  auto http_conn = new HTTPConnection(
      handler_factory,
      std::move(conn),
      scheduler);

  http_conn->nextRequest();
}

HTTPConnection::HTTPConnection(
    HTTPHandlerFactory* handler_factory,
    std::unique_ptr<net::TCPConnection> conn,
    thread::TaskScheduler* scheduler) :
    handler_factory_(handler_factory),
    conn_(std::move(conn)),
    scheduler_(scheduler),
    on_write_completed_cb_(nullptr),
    refcount_(1),
    read_body_(false) {
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

  parser_.onHeadersComplete(std::bind(&HTTPConnection::dispatchRequest, this));
}

void HTTPConnection::read() {
  size_t len;
  try {
    len = conn_->read(buf_.data(), buf_.allocSize());
    iputs("read fired: $0", buf_.toString());
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      decRef();
      return awaitRead();
    }

    log::Logger::get()->logException(
        fnord::log::kDebug,
        "HTTP read() failed, closing connection",
        e);

    iputs("close b/c read error...\n", 1);
    close();
    return;
  }

  try {
    if (len == 0) {
      parser_.eof();
      iputs("close b/c eof...\n", 1);
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

    iputs("close b/c parse error...\n", 1);
    close();
    return;
  }

  if (read_body_) {
    if (parser_.state() != HTTPParser::S_DONE) {
      awaitRead();
    }
  } else {
    if (parser_.state() < HTTPParser::S_BODY) {
      awaitRead();
    }
  }

  if (decRef()) {
    return;
  }
}

void HTTPConnection::write() {
  auto data = ((char *) buf_.data()) + buf_.mark();
  auto size = buf_.size() - buf_.mark();

  //iputs("write fired: $0", buf_.toString());

  size_t len;
  try {
    len = conn_->write(data, size);
    iputs("writing: $1 ($3/$4) >>$0<< from ($5) >>$2<< ", std::string(data, len), buf_.mark(), buf_.toString(), data, ((char *) buf_.data()) + buf_.mark(), buf_.size());
    buf_.setMark(buf_.mark() + len);
  } catch (Exception& e) {
    if (e.ofType(kWouldBlockError)) {
      //decRef();
      //return awaitWrite();
    }

    log::Logger::get()->logException(
        fnord::log::kDebug,
        "HTTP write() failed, closing connection",
        e);

    iputs("close b/c write error...\n", 1);
    close();
    return;
  }

  if (buf_.mark() < buf_.size()) {
    awaitWrite();
  } else {
    iputs("write done!!", 1);
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

void HTTPConnection::nextRequest() {
  iputs("next request...\n", 1);

  parser_.reset();
  cur_request_.reset(new HTTPRequest());
  body_buf_.clear();

  on_write_completed_cb_ = nullptr;
  awaitRead();
}

void HTTPConnection::dispatchRequest() {
  incRef();
  cur_handler_= handler_factory_->getHandler(this, cur_request_.get());
  cur_handler_->handleHTTPRequest();
}

void HTTPConnection::readRequestBody(
    std::function<void (const void*, size_t, bool)> callback) {

/*
  auto read_body_chunk = [this, callback] () {
    iputs("read body called...\n", 1);
    bool last_chunk;

    switch (parser_.state()) {
      case HTTPParser::S_METHOD:
      case HTTPParser::S_URI:
      case HTTPParser::S_VERSION:
      case HTTPParser::S_HEADER:
        RAISE(kIllegalStateError, "can't read body before headers are parsed");
      case HTTPParser::S_BODY:
        last_chunk = false;
        break;
      case HTTPParser::S_DONE:
        last_chunk = true;
        break;
    }
    iputs("readbody state=$0 last=$1", (int) parser_.state(), last_chunk);

    // FIXPAUL handle exceptions?
    callback(body_buf_.data(), body_buf_.size(), last_chunk);
  };
*/
  if (parser_.state() == HTTPParser::S_DONE) {
    callback(body_buf_.data(), body_buf_.size(), true);
  } else {
    abort();
    awaitRead();
  }
}

void HTTPConnection::writeResponse(
    const HTTPResponse& resp,
    std::function<void()> ready_callback) {
  decRef();
  buf_.clear();
  io::BufferOutputStream os(&buf_);
  HTTPGenerator::generate(resp, &os);
  on_write_completed_cb_ = ready_callback;
  incRef();
  awaitWrite();
}

void HTTPConnection::writeResponseBody(
    const void* data,
    size_t size,
    std::function<void()> ready_callback) {
}

void HTTPConnection::finishResponse() {
  decRef();

  if (cur_request_->keepalive()) {
    nextRequest();
  } else {
    iputs("close b/c no keepalive...\n", 1);
    close();
  }
}

void HTTPConnection::close() {
  iputs("close...\n", 1);
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

