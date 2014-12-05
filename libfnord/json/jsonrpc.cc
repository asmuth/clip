/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpcrequest.h"
#include "fnord/json/jsonrpcresponse.h"

namespace fnord {
namespace json {

JSONRPC::JSONRPC() {}

void JSONRPC::dispatch(JSONRPCRequest* req, JSONRPCResponse* res) {
  auto iter = handlers_.find(req->method());
  if (iter == handlers_.end()) {
    res->error(
        JSONRPCResponse::kJSONRPCPMethodNotFoundError,
        "method not found: " + req->method());
  } else {
    iter->second(req, res);
  }
}

void JSONRPC::registerMethod(
    const std::string& method,
    std::function<void (JSONRPCRequest* req, JSONRPCResponse* res)> handler) {
  handlers_[method] = handler;
}

} // namespace json
} // namsepace fnord

