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
#pragma once
#include "libtransport/http/http_request.h"
#include "libtransport/http/v1/http_connection.h"

namespace libtransport {
namespace http {

class SSEGenerator {
public:

  /**
   * Initialize the response from the provided request, write to the
   * provided connection
   */
  SSEGenerator(
      const HTTPRequest& request,
      HTTPConnection* connection);

  bool start();

  bool sendEvent(
      const std::string& event_data,
      const std::string& event_type = "");

private:
  HTTPResponse res_;
  HTTPConnection* conn_;
};

} // namespace http
} // namespace libtransport

