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
#include "libtransport/http/sse_generator.h"

namespace libtransport {
namespace http {

SSEGenerator::SSEGenerator(
    const HTTPRequest& request,
    HTTPConnection* connection) :
    conn_(connection) {
  res_.populateFromRequest(request);
  res_.setStatus(kStatusOK);
  res_.setHeader("Connection", "close");
  res_.setHeader("Content-Type", "text/event-stream");
  res_.setHeader("Cache-Control", "no-cache");
  res_.setHeader("Access-Control-Allow-Origin", "*");
}

bool SSEGenerator::start() {
  return conn_->sendResponseHeaders(res_);
}

bool SSEGenerator::sendEvent(
    const std::string& event_data,
    const std::string& event_type /* = "" */) {
  std::string buf;

  if (!event_type.empty()) {
    buf.append("event: ");
    buf.append(event_type);
    buf.append("\n");
  }

  buf.append("data: ");
  buf.append(event_data);
  buf.append("\n\n");

  return conn_->sendResponseBodyChunk(buf.data(), buf.size());
}

} // namespace http
} // namespace libtransport

