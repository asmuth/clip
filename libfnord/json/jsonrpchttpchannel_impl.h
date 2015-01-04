/*
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_JSONRPCHTTPCHANNELIMPL_H
#define _FNORD_JSON_JSONRPCHTTPCHANNELIMPL_H
#include "fnord/base/exception.h"
#include "fnord/base/inspect.h"
#include "fnord/json/json.h"
#include "fnord/json/jsonrpcrequest.h"

namespace fnord {
namespace json {

template <class RPCType>
void JSONRPCHTTPChannel::call(RPCType* rpc) {
  JSONObject req;
  req.emplace_back(JSON_OBJECT_BEGIN);
  req.emplace_back(JSON_STRING, "jsonrpc");
  req.emplace_back(JSON_STRING, "2.0");
  req.emplace_back(JSON_STRING, "method");
  req.emplace_back(JSON_STRING, rpc->method());
  req.emplace_back(JSON_STRING, "id");
  req.emplace_back(JSON_STRING, "0"); // FIXPAUL
  req.emplace_back(JSON_STRING, "params");
  fnord::json::toJSON(rpc->args(), &req);
  req.emplace_back(JSON_OBJECT_END);

  auto future = call(req);

  future.onSuccess([rpc] (const JSONObject& res) {
    try {
      auto err_iter = JSONUtil::objectLookup(res.begin(), res.end(), "error");
      if (err_iter != res.end()) {
        auto err_str_i = JSONUtil::objectLookup(err_iter, res.end(), "message");
        if (err_str_i == res.end()) {
          RAISE(kRPCError, "invalid JSONRPC response");
        } else {
          RAISE(kRPCError, err_str_i->data);
        }
      }

      auto res_iter = JSONUtil::objectLookup(res.begin(), res.end(), "result");
      rpc->ready(fromJSON<typename RPCType::ResultType>(res_iter, res.end()));
    } catch (const std::exception& e) {
      rpc->error(e);
    }
  });

  future.onFailure([rpc] (const Status& status) {
    rpc->error(status);
  });
}

} // namespace json
} // namsepace fnord
#endif
