/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_JSONRPCHTTPADAPTER_H
#define _FNORD_JSON_JSONRPCHTTPADAPTER_H
#include <functional>
#include <stdlib.h>
#include <string>
#include <vector>
#include "fnord/net/http/httprequest.h"
#include "fnord/net/http/httpresponse.h"
#include "fnord/net/http/httpservice.h"

namespace fnord {
namespace json {
class JSONRPC;

class JSONRPCHTTPAdapter : public fnord::http::HTTPService {
public:

  static std::unique_ptr<http::HTTPService> make(JSONRPC* rpc);

  JSONRPCHTTPAdapter(
      JSONRPC* json_rpc,
      const std::string path= "/rpc");

  void handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response) override;

protected:
  JSONRPC* json_rpc_;
  std::string path_;
};

} // namespace json
} // namsepace fnord
#endif
