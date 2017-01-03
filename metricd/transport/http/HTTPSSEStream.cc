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
