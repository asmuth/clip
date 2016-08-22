/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
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
#ifndef _libstx_HTTP_STATUSES_H
#define _libstx_HTTP_STATUSES_H

namespace fnordmetric {
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
const HTTPStatus kStatusMovedPermanently(301, "Moved permanently");
const HTTPStatus kStatusFound(302, "Found");
const HTTPStatus kStatusInternalServerError(500, "Internal Server Error");
const HTTPStatus kStatusBadGateway(502, "Bad Gateway");
const HTTPStatus kStatusServiceUnavailable(503, "Service unavailable");

}
}
#endif
