/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/json/jsonrpcrequest.h"
#include "fnord/json/jsonrpcresponse.h"
#include "fnord/service/keyvalue/keyvalueservice.h"
#include "fnord/service/keyvalue/keyvalueserviceadapter.h"

namespace fnord {
namespace keyvalue_service {

void KeyValueServiceAdapter::registerJSONRPC(
    KeyValueService* keyvalue_service,
    json::JSONRPC* rpc) {

  rpc->registerMethod("KeyValueService.get", [keyvalue_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    auto p_key = req->body().get("/params/key");

    std::string value;
    if (keyvalue_service->get(p_key, &value)) {
      res->successAndReturn(value);
    } else {
      res->successAndReturn(nullptr);
    }
  });

  rpc->registerMethod("KeyValueService.mget", [keyvalue_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    std::vector<std::string> p_keys;
    req->body().forEach("/params/keys", [&] (
        const json::JSONPointer& path) -> bool {
      p_keys.emplace_back(req->body().get(path));
      return true;
    });

    res->successAndReturn(keyvalue_service->mget(p_keys));
  });

  rpc->registerMethod("KeyValueService.set", [keyvalue_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    auto p_key = req->body().get("/params/key");
    auto p_value = req->body().get("/params/value");

    keyvalue_service->set(p_key, p_value);
    res->successAndReturn(true);
  });
}

} // namespace keyvalue_service
} // namsepace fnord

