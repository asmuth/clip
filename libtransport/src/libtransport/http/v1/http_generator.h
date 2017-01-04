/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <vector>
#include <string>
#include <utility>
#include <libtransport/http/http_response.h>
#include <libtransport/http/http_request.h>

namespace libtransport {
namespace http {

class HTTPGenerator {
public:
  static bool generateHeaders(const HTTPRequest& req, std::string* os);
  static bool generate(const HTTPRequest& req, std::string* os);
  static bool generateHeaders(const HTTPResponse& res, std::string* os);
  static bool generate(const HTTPResponse& res, std::string* os);
};

} // namespace http
} // namespace libtransport

