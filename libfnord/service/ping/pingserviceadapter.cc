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
#include "fnord/service/ping/pingservice.h"
#include "fnord/service/ping/pingserviceadapter.h"

namespace fnord {
namespace ping_service {

void PingServiceAdapter::registerJSONRPC(
    PingService* ping_service,
    json::JSONRPC* rpc) {

  rpc->registerMethod("PingService.ping", [ping_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    res->successAndReturn(ping_service->ping());
  });
}

} // namespace ping_service
} // namsepace fnord

