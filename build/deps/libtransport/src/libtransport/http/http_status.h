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

namespace libtransport {
namespace http {

struct HTTPStatus {
  HTTPStatus(int code_, const char* name_) : code(code_), name(name_) {}
  int code;
  const char* name;
};

const HTTPStatus kStatusOK(200, "OK");
const HTTPStatus kStatusCreated(201, "Created");
const HTTPStatus kStatusNoContent(204, "No Content");
const HTTPStatus kStatusBadRequest(400, "Bad request");
const HTTPStatus kStatusUnauthorized(401, "Unauthorized");
const HTTPStatus kStatusForbidden(403, "Forbidden");
const HTTPStatus kStatusNotFound(404, "Not found");
const HTTPStatus kStatusMethodNotAllowed(405, "Method not allowed");
const HTTPStatus kStatusMovedPermanently(301, "Moved permanently");
const HTTPStatus kStatusFound(302, "Found");
const HTTPStatus kStatusInternalServerError(500, "Internal Server Error");
const HTTPStatus kStatusBadGateway(502, "Bad Gateway");
const HTTPStatus kStatusServiceUnavailable(503, "Service unavailable");

} // namespace http
} // namespace libtransport

