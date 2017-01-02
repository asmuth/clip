/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/transport/http/HTTPRequestStream.h>

namespace fnordmetric {
namespace http {

HTTPRequestStream::HTTPRequestStream(
    const HTTPRequest& req,
    RefPtr<HTTPServerConnection> conn) :
    req_(req),
    conn_(conn) {}

const HTTPRequest& HTTPRequestStream::request() const {
  return req_;
}

void HTTPRequestStream::readBody(Function<void (const void*, size_t)> fn) {
  RefPtr<Wakeup> wakeup(new Wakeup());
  bool error = false;

  conn_->readRequestBody([this, fn, wakeup] (
      const void* data,
      size_t size,
      bool last_chunk) {
    fn(data, size);

    if (last_chunk) {
      wakeup->wakeup();
    }
  },
  [&error, &wakeup] {
    error = true;
    wakeup->wakeup();
  });

  wakeup->waitForFirstWakeup();

  if (error) {
    RAISE(kIOError, "client error");
  }
}

void HTTPRequestStream::readBody() {
  readBody([this] (const void* data, size_t size) {
    req_.appendBody(data, size);
  });
}

void HTTPRequestStream::discardBody() {
  readBody([this] (const void* data, size_t size) {});
}

}
}
