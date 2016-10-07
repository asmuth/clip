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
