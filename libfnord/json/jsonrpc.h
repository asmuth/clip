/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_JSONRPC_H
#define _FNORD_JSON_JSONRPC_H
#include <functional>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord/json/jsonrpchttpadapter.h"

namespace fnord {
namespace json {
class JSONRPCRequest;
class JSONRPCResponse;

class JSONRPC {
public:

  JSONRPC();

  void dispatch(JSONRPCRequest* req, JSONRPCResponse* res);

  void registerMethod(
      const std::string& method,
      std::function<void (JSONRPCRequest* req, JSONRPCResponse* res)> handler);

protected:
  std::unordered_map<
      std::string,
      std::function<void (JSONRPCRequest* req, JSONRPCResponse* res)>>
      handlers_;
};

} // namespace json
} // namsepace fnord
#endif
