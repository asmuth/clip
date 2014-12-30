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
    fnord::thread::TaskScheduler* scheduler,
    const std::string path /* = "/rpc" */) :
    http_chan_(lb_group, scheduler),
    scheduler_(scheduler),
    path_(path) {}

void JSONRPCHTTPChannel::call(
    const JSONObject& json_req,
    std::function<void (const JSONObject& res)> on_success,
    std::function<void (const std::exception& e)> on_error) {
  std::unique_ptr<http::HTTPResponseFuture> http_future;

  try {
    http::HTTPRequest http_req(http::HTTPRequest::M_GET, path_);
    JSONOutputStream json(http_req.getBodyOutputStream());
    json.write(json_req);
    http_req.setHeader(
        "Content-Length",
        StringUtil::toString(http_req.body().size()));

    http_future = http_chan_.executeRequest(http_req);
  } catch (std::exception& e) {
    on_error(e);
    return;
  }

  auto req_handle = new RequestHandle();
  req_handle->http_future = std::move(http_future);
  req_handle->on_success = on_success;
  req_handle->on_error = on_error;

  auto http_ready = [req_handle] {
    std::unique_ptr<RequestHandle> autodelete(req_handle);
    const auto& http_req = req_handle->http_future->get();
    JSONObject res;

    try {
      auto res = parseJSON(http_req.body());
    } catch (std::exception& e) {
      req_handle->on_error(e);
      return;
    }

    req_handle->on_success(res);
  };

  try {
    scheduler_->runOnWakeup(http_ready, req_handle->http_future->onReady(), 0);
  } catch (std::exception& e) {
    on_error(e);
    delete req_handle;
    return;
  }
}

} // namespace json
} // namsepace fnord
