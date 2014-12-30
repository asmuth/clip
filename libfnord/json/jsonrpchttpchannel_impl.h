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
  req.emplace_back(JSON_STRING, method_prefix_ + rpc->method());
  req.emplace_back(JSON_STRING, "id");
  req.emplace_back(JSON_STRING, "0"); // FIXPAUL
  req.emplace_back(JSON_STRING, "params");
  fnord::json::toJSON(rpc->args(), &req);
  req.emplace_back(JSON_OBJECT_END);

  auto on_success = [rpc] (const JSONObject& res) {
    auto err_iter = JSONUtil::objectLookup(res.begin(), res.end(), "error");
    if (err_iter != res.end()) {
      std::string err_str;

      auto err_str_i = JSONUtil::objectLookup(err_iter, res.end(), "message");
      if (err_str_i == res.end()) {
        err_str = "invalid JSONRPC response";
      } else {
        err_str = err_str_i->data;
      }

      fnord::Exception e(err_str);
      e.setTypeName(kRPCError);
      rpc->error(e);
      return;
    }
  };

  auto on_error = [rpc] (const std::exception& e) {
    rpc->error(e);
  };

  call(req, on_success, on_error);
}

} // namespace json
} // namsepace fnord
#endif
