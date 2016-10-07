/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *   - Laura Schlimmer <laura@zscale.io>
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
#include <metricd/util/inspect.h>
#include <metricd/util/exception.h>
#include <metricd/transport/http/HTTPResponseStream.h>

namespace fnordmetric {
namespace http {

HTTPResponseStream::HTTPResponseStream(
    RefPtr<HTTPServerConnection> conn) :
    conn_(conn),
    callback_running_(false),
    headers_written_(false),
    response_finished_(false),
    error_(false) {}

void HTTPResponseStream::writeResponse(HTTPResponse res) {
  auto body_size = res.body().size();
  if (body_size > 0) {
    res.setHeader("Content-Length", StringUtil::toString(body_size));
  }

  startResponse(res);
  finishResponse();
}

void HTTPResponseStream::startResponse(const HTTPResponse& resp) {
  std::unique_lock<std::mutex> lk(mutex_);

  if (headers_written_) {
    RAISE(kRuntimeError, "headers already written");
  }

  if (error_) {
    RAISE(kIOError, "client error");
  }

  headers_written_ = true;
  callback_running_ = true;
  lk.unlock();

  incRef();
  conn_->writeResponse(
      resp,
      std::bind(&HTTPResponseStream::onCallbackCompleted, this),
      std::bind(&HTTPResponseStream::onCallbackError, this));
}

void HTTPResponseStream::writeBodyChunk(const Buffer& buf) {
  writeBodyChunk(buf.data(), buf.size());
}

void HTTPResponseStream::writeBodyChunk(const void* data, size_t size) {
  std::unique_lock<std::mutex> lk(mutex_);
  if (error_) {
    RAISE(kIOError, "client error");
  }

  buf_.append(data, size);
  onStateChanged(&lk);
}

void HTTPResponseStream::finishResponse() {
  std::unique_lock<std::mutex> lk(mutex_);

  if (response_finished_) {
    return;
  }

  if (error_) {
    RAISE(kIOError, "client error");
  }

  response_finished_ = true;
  onStateChanged(&lk);
}

bool HTTPResponseStream::isOutputStarted() const {
  std::unique_lock<std::mutex> lk(mutex_);
  return headers_written_;
}

bool HTTPResponseStream::isClosed() const {
  return error_ || conn_->isClosed();
}

void HTTPResponseStream::onCallbackCompleted() {
  {
    std::unique_lock<std::mutex> lk(mutex_);
    callback_running_ = false;
    onStateChanged(&lk);
  }

  decRef();
}

void HTTPResponseStream::onCallbackError() {
  std::unique_lock<std::mutex> lk(mutex_);
  callback_running_ = false;
  error_ = true;
  lk.unlock();
  cv_.notify_all();
  decRef();
}

void HTTPResponseStream::onBodyWritten(Function<void ()> callback) {
  std::unique_lock<std::mutex> lk(mutex_);
  on_body_written_ = callback;
}

size_t HTTPResponseStream::bufferSize() {
  std::unique_lock<std::mutex> lk(mutex_);
  return buf_.size();
}

void HTTPResponseStream::waitForReader() {
  std::unique_lock<std::mutex> lk(mutex_);
  auto selfref = mkRef(this);

  while (!error_ && buf_.size() > kMaxWriteBufferSize) {
    cv_.wait(lk);
  }

  if (error_) {
    RAISE(kIOError, "client error");
  }
}

// precondition: lk must be locked
void HTTPResponseStream::onStateChanged(std::unique_lock<std::mutex>* lk) {
  if (callback_running_) {
    return;
  }

  if (!headers_written_) {
    return; // should never happen!
  }

  if (buf_.size() > 0) {
    Buffer write_buf = buf_;
    buf_.clear();

    if (write_buf.size() > kMaxWriteBufferSize) {
      buf_.append(
          (char*) write_buf.data() + kMaxWriteBufferSize,
          write_buf.size() - kMaxWriteBufferSize);

      write_buf.truncate(kMaxWriteBufferSize);
    }

    cv_.notify_all();
    callback_running_ = true;
    lk->unlock();

    incRef();
    conn_->writeResponseBody(
        write_buf.data(),
        write_buf.size(),
        std::bind(&HTTPResponseStream::onCallbackCompleted, this),
        std::bind(&HTTPResponseStream::onCallbackError, this));

    return;
  }

  if (response_finished_) {
    conn_->finishResponse();
    lk->unlock();
  } else {
    lk->unlock();

    if (on_body_written_) {
      on_body_written_();
    }
  }
}

}
}
