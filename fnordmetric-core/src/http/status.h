/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTP_STATUSES_H
#define _FNORDMETRIC_HTTP_STATUSES_H

namespace fnord {
namespace http {

struct HTTPStatus {
  HTTPStatus(int code_, const char* name_) : code(code_), name(name_) {}
  int code;
  const char* name;
};

const HTTPStatus kStatusOK(200, "OK");
const HTTPStatus kStatusCreated(201, "Created");
const HTTPStatus kStatusBadRequest(404, "Bad request");
const HTTPStatus kStatusNotFound(404, "Not found");
const HTTPStatus kStatusMovedPermanently(301, "Moved permanently");
const HTTPStatus kStatusFound(302, "Found");
const HTTPStatus kStatusInternalServerError(500, "InternalServerError");

}
}
#endif
