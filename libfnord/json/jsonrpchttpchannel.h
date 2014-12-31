/*
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_JSONRPCHTTPCHANNEL_H
#define _FNORD_JSON_JSONRPCHTTPCHANNEL_H
#include <functional>
#include <stdlib.h>
#include <string>
#include <vector>
#include "fnord/comm/lbgroup.h"
#include "fnord/comm/rpcchannel.h"
#include "fnord/json/json.h"
#include "fnord/json/jsonrpcrequest.h"
#include "fnord/net/http/httpconnectionpool.h"
#include "fnord/net/http/httprequest.h"
#include "fnord/net/http/httpresponse.h"

namespace fnord {
namespace json {

class JSONRPCHTTPChannel : public comm::RPCChannel {
public:

  JSONRPCHTTPChannel(
      comm::LBGroup* lb_group,
      fnord::thread::TaskScheduler* scheduler);

  template <class RPCType>
  void call(RPCType* rpc);

  void call(
      const JSONObject& json_req,
      std::function<void (const JSONObject& res)> on_success,
      std::function<void (const std::exception& e)> on_error);

protected:
  struct RequestHandle {
    std::unique_ptr<http::HTTPResponseFuture> http_future;
    std::function<void (const JSONObject& res)> on_success;
    std::function<void (const std::exception& e)> on_error;
  };

  comm::LBGroup* lb_group_;
  http::HTTPConnectionPool conn_pool_;
  thread::TaskScheduler* scheduler_;
};

} // namespace json
} // namsepace fnord
#endif

#include "fnord/comm/rpc.h"
#include "jsonrpchttpchannel_impl.h"
