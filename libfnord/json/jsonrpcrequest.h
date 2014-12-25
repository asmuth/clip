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
#include "fnord/base/inspect.h"
#include "fnord/json/jsondocument.h"
#include "fnord/json/jsoninputstream.h"

namespace fnord {
namespace json {

class JSONRPCRequest {
public:

  JSONRPCRequest(JSONInputStream&& input);

  const JSONDocument& body() const;
  std::string id() const;
  std::string method() const;

  template <typename T>
  T getArg(size_t index, const std::string& name) const {
    fnord::iputs("getarg: $0, $1", index, name);
    return 4444;
  };

protected:
  JSONDocument body_;
};

} // namespace json
} // namsepace fnord
#endif
