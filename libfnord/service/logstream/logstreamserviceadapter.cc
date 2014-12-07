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
#include "fnord/service/logstream/logstreamservice.h"
#include "fnord/service/logstream/logstreamserviceadapter.h"

namespace fnord {
namespace logstream_service {

void LogStreamServiceAdapter::registerJSONRPC(
    LogStreamService* logstream_service,
    json::JSONRPC* rpc) {

  rpc->registerMethod("LogStreamService.append", [logstream_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    uint64_t offset = 42;
    res->successAndReturn(offset);
  });
}

} // namespace logstream_service
} // namsepace fnord

