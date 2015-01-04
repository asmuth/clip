/*
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/json/jsonrpchttpchannel.h"

namespace fnord {
namespace json {

JSONRPCHTTPChannel::JSONRPCHTTPChannel(
    comm::LBGroup* lb_group,
    fnord::TaskScheduler* scheduler) :
    lb_group_(lb_group),
    conn_pool_(scheduler),
    scheduler_(scheduler) {}

Future<JSONObject> JSONRPCHTTPChannel::call(const JSONObject& json_req) {
  Promise<JSONObject> promise;

  auto on_http_success = [&promise] (const http::HTTPResponse& http_res) {
    JSONObject res;

    if (http_res.statusCode() != 200) {
      promise.failure(
          Status(
              eRPCError,
              StringUtil::format(
                  "JSONRPC received non 200 HTTP status code: $0",
                  http_res.body().toString())));
      return;
    }

    try {
      res = parseJSON(http_res.body());
    } catch (std::exception& e) {
      promise.failure(e);
      return;
    }

    promise.success(res);
  };

  try {
    URI server(lb_group_->getServerForNextRequest());
    auto addr = fnord::net::InetAddr::resolve(server.hostAndPort());
    if (!addr.hasPort()) {
      addr.setPort(80);
    }

    http::HTTPRequest http_req(http::HTTPRequest::M_POST, server.path());
    JSONOutputStream json(http_req.getBodyOutputStream());
    json.write(json_req);
    http_req.setHeader(
        "Content-Length",
        StringUtil::toString(http_req.body().size()));

    if (!http_req.hasHeader("Host")) {
      http_req.setHeader("Host", server.hostAndPort());
    }

    auto http_future = conn_pool_.executeRequest(http_req, addr);
    http_future.onSuccess(on_http_success);
    http_future.onFailure([&promise] (const Status& status) {
      promise.failure(status);
    });
  } catch (std::exception& e) {
    promise.failure(e);
  }

  return promise.future();
}

} // namespace json
} // namsepace fnord
