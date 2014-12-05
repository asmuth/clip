/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_JSONRPCREQUEST_H
#define _FNORD_JSON_JSONRPCREQUEST_H
#include <stdlib.h>
#include <string>
#include <vector>
#include "fnord/format/json/jsondocument.h"
#include "fnord/format/json/jsoninputstream.h"

namespace fnord {
namespace json {

class JSONRPCRequest {
public:

  JSONRPCRequest(JSONInputStream&& input);

  const JSONDocument& body() const;
  std::string id() const;
  std::string method() const;

protected:
  JSONDocument body_;
};

} // namespace json
} // namsepace fnord
#endif
