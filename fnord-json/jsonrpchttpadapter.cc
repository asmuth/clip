/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/inspect.h"
#include "fnord-base/stringutil.h"
#include "fnord/json/json.h"
#include "fnord/json/jsonoutputstream.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "fnord/json/jsonrpcresponse.h"
#include "fnord/json/jsonrpcrequest.h"

namespace fnord {
namespace json {

std::unique_ptr<http::HTTPService> JSONRPCHTTPAdapter::make(JSONRPC* rpc) {
  return std::unique_ptr<http::HTTPService>(new JSONRPCHTTPAdapter(rpc));
}

JSONRPCHTTPAdapter::JSONRPCHTTPAdapter(
    JSONRPC* json_rpc,
    const std::string path /* = "/rpc" */) :
    json_rpc_(json_rpc),
    path_(path) {
  StringUtil::stripTrailingSlashes(&path_);
}

void JSONRPCHTTPAdapter::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  if (!StringUtil::beginsWith(request->uri(), path_)) {
    response->setStatus(http::kStatusNotFound);
    response->addBody("not found");
    return;
  }

  response->setStatus(http::kStatusOK);
  JSONRPCResponse res(response->getBodyOutputStream());

  if (request->method() != http::HTTPRequest::M_POST) {
    res.error(
        JSONRPCResponse::kJSONRPCPInvalidRequestError,
        "HTTP method must be POST");
    return;
  }

  try {
    JSONRPCRequest req(parseJSON(request->body()));
    res.setID(req.id());
    json_rpc_->dispatch(&req, &res);
  } catch (const fnord::Exception& e) {
    res.error(
        JSONRPCResponse::kJSONRPCPInternalError,
        e.getMessage());
  }
}


} // namespace json
} // namsepace fnord

