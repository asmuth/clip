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

#include "HTTPSSEStream.h"

namespace fnordmetric {
namespace http {

HTTPSSEStream::HTTPSSEStream(
    RefPtr<http::HTTPRequestStream> req_stream,
    RefPtr<http::HTTPResponseStream> res_stream) :
    res_stream_(res_stream) {
  req_stream->readBody();
  res_.populateFromRequest(req_stream->request());
}

HTTPSSEStream::HTTPSSEStream(
    const http::HTTPRequest* req,
    RefPtr<http::HTTPResponseStream> res_stream) :
    res_stream_(res_stream) {
  res_.populateFromRequest(*req);
}

HTTPSSEStream::HTTPSSEStream(
    const http::HTTPResponse* res,
    RefPtr<http::HTTPResponseStream> res_stream) : 
    res_stream_(res_stream),
    res_(*res) {}

void HTTPSSEStream::start() {
  res_.setStatus(kStatusOK);
  res_.setHeader("Connection", "close");
  res_.setHeader("Content-Type", "text/event-stream");
  res_.setHeader("Cache-Control", "no-cache");
  res_.setHeader("Access-Control-Allow-Origin", "*");
  res_stream_->startResponse(res_);
}

void HTTPSSEStream::sendEvent(
    const String& data,
    const Option<String>& event_type) {
  sendEvent(data.data(), data.size(), event_type);
}

void HTTPSSEStream::sendEvent(
    const Buffer& data,
    const Option<String>& event_type) {
  sendEvent(data.data(), data.size(), event_type);
}

void HTTPSSEStream::sendEvent(
    const void* event_data,
    size_t event_size,
    const Option<String>& event_type) {
  Buffer buf;
  buf.reserve(event_size + 1024);

  if (!event_type.isEmpty()) {
    buf.append("event: ");
    buf.append(event_type.get());
    buf.append("\n");
  }

  buf.append("data: ");
  buf.append(event_data, event_size);
  buf.append("\n\n");

  res_stream_->writeBodyChunk(buf);
}

void HTTPSSEStream::finish() {
  res_stream_->finishResponse();
}

const HTTPResponse HTTPSSEStream::response() const {
  return res_;
}

bool HTTPSSEStream::isClosed() const {
  return res_stream_->isClosed();
}

}
}
