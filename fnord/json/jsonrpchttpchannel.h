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
#include "fnord-rpc/ServerGroup.h"
#include "fnord/comm/rpcchannel.h"
#include "fnord/json/json.h"
#include "fnord/json/jsonrpcrequest.h"
#include "fnord/net/http/httpconnectionpool.h"
#include "fnord/net/http/httprequest.h"
#include "fnord/net/http/httpresponse.h"
#include "fnord/base/thread/future.h"

namespace fnord {
namespace json {

class JSONRPCHTTPChannel : public comm::RPCChannel {
public:

  JSONRPCHTTPChannel(
      comm::LBGroup* lb_group,
      fnord::TaskScheduler* scheduler);

  template <class RPCType>
  void call(RPCType* rpc);

  Future<JSONObject> call(const JSONObject& json_req);

  http::HTTPConnectionPool* httpConnectionPool();

protected:
  comm::LBGroup* lb_group_;
  http::HTTPConnectionPool conn_pool_;
  TaskScheduler* scheduler_;
};

} // namespace json
} // namsepace fnord
#endif

#include "fnord-rpc/RPC.h"
#include "jsonrpchttpchannel_impl.h"
