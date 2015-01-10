/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/stats/statshttpservlet.h"

namespace fnord {
namespace stats {

void StatsHTTPServlet::handleHTTPRequest(
    fnord::http::HTTPRequest* req,
    fnord::http::HTTPResponse* res) {
  auto res_body = fnord::StringUtil::format(
      "pong: $0",
      req->body().toString());

  res->setStatus(fnord::http::kStatusOK);
  res->addBody(res_body);
}

}
}
