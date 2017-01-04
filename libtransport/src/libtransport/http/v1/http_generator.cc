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
#include <libtransport/http/v1/http_generator.h>

namespace libtransport {
namespace http {

bool HTTPGenerator::generateHeaders(const HTTPRequest& req, std::string* os) {
  os->append(getHTTPMethodName(req.method()));
  os->append(" ");
  os->append(req.uri());
  os->append(" ");
  os->append(req.version());
  os->append("\r\n");

  for (const auto& header : req.headers()) {
    os->append(header.first);
    os->append(": ");
    os->append(header.second);
    os->append("\r\n");
  }

  if (req.body().size() > 0 && !req.hasHeader("Content-Length")) {
    os->append("Content-Length: ");
    os->append(std::to_string(req.body().size()));
    os->append("\r\n");
  }

  os->append("\r\n");
  return true;
}

bool HTTPGenerator::generate(const HTTPRequest& req, std::string* os) {
  if (!generateHeaders(req, os)) {
    return false;
  }

  const auto& body = req.body();
  if (body.size() > 0) {
    os->append(body);
  }

  return true;
}

bool HTTPGenerator::generateHeaders(const HTTPResponse& res, std::string* os) {
  if (res.version().length() < 4) {
    return false;
  }

  os->append(res.version());
  os->append(" ");
  os->append(std::to_string(res.statusCode()));
  os->append(" ");
  os->append(res.statusName());
  os->append("\r\n");

  for (const auto& header : res.headers()) {
    os->append(header.first);
    os->append(": ");
    os->append(header.second);
    os->append("\r\n");
  }

  if (res.body().size() > 0 && !res.hasHeader("Content-Length")) {
    os->append("Content-Length: ");
    os->append(std::to_string(res.body().size()));
    os->append("\r\n");
  }

  os->append("\r\n");
  return true;
}

bool HTTPGenerator::generate(const HTTPResponse& res, std::string* os) {
  if (!generateHeaders(res, os)) {
    return false;
  }

  const auto& body = res.body();
  if (body.size() > 0) {
    os->append(body);
  }

  return true;
}

} // namespace http
} // namespace libtransport

