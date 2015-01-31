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
#include "fnord-base/inspect.h"
#include "fnord-json/json.h"

namespace fnord {
namespace json {

class JSONRPCRequest {
public:
  JSONRPCRequest(JSONObject&& body);

  const std::string& id() const;
  const std::string& method() const;

  JSONObject::const_iterator paramsBegin() const;
  JSONObject::const_iterator paramsEnd() const;

  template <typename T>
  T getArg(size_t index, const std::string& name) const;

protected:
  JSONObject body_;
  JSONObject::const_iterator params_;
  std::string id_;
  std::string method_;
};

} // namespace json
} // namsepace fnord

#include "jsonrpcrequest_impl.h"
#endif
