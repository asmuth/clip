/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/logging.h"
#include "fnord-rpc/RPCClient.h"

namespace fnord {

HTTPRPCClient::HTTPRPCClient(
    TaskScheduler* sched) :
    http_pool_(sched) {}

void HTTPRPCClient::call(const URI& uri, RefPtr<AnyRPC> rpc) {
  http::HTTPRequest http_req(http::HTTPRequest::M_POST, uri.path());

  http_req.addBody(rpc->encoded());
  http_req.setHeader("Host", uri.hostAndPort());

  auto http_future = http_pool_.executeRequest(http_req);

#ifndef FNORD_NOTRACE
  fnord::logTrace(
      "fnord.http.rpcclient",
      "executing RPC via HTTP request id=$2 method=$1 uri=$0",
      uri.toString(),
      rpc->method(),
      (void*) rpc.get());
#endif

  http_future.onSuccess([uri, rpc] (const http::HTTPResponse& resp) {
    if (resp.statusCode() < 200 || resp.statusCode() >= 300) {
      auto errmsg = StringUtil::format(
          "got non-200 HTTP response code: $0",
          resp.body().toString());

#ifndef FNORD_NOTRACE
    fnord::logTrace(
        "fnord.http.rpcclient",
        "RPC via HTTP request id=$0: Failed: $1",
        (void*) rpc.get(),
        errmsg);
#endif

      rpc->error(Status(eRPCError, errmsg));
      return;
    }

#ifndef FNORD_NOTRACE
    fnord::logTrace(
        "fnord.http.rpcclient",
        "RPC via HTTP request id=$0: Success, got $1 bytes response",
        (void*) rpc.get(),
        resp.body().size());
#endif
    rpc->ready(resp.body());
  });

  http_future.onFailure([uri, rpc] (const Status& status) mutable{
#ifndef FNORD_NOTRACE
    fnord::logTrace(
        "fnord.http.rpcclient",
        "RPC via HTTP request id=$0: Failed: $1",
        (void*) rpc.get(),
        status);
#endif

    rpc->error(status);
  });
}

} // namespace fnord

