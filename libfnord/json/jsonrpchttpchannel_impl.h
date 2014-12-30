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
  req.emplace_back(JSON_OBJECT_END);

  auto on_success = [rpc] (const JSONObject& res) {
    fnord::iputs("yeahh!!", 1);
  };

  auto on_error = [rpc] (const std::exception& e) {
    rpc->error(e);
  };

  call(req, on_success, on_error);
}

} // namespace json
} // namsepace fnord
#endif
