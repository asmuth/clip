/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/exception.h"
#include "fnord/json/jsonrpcrequest.h"

namespace fnord {
namespace json {

JSONRPCRequest::JSONRPCRequest(
    JSONInputStream&& input) :
    body_(std::move(input)) {
  if (body_.get("/jsonrpc") != "2.0") {
    RAISE(kRuntimeError, "invalid JSONRPC 2.0 request, missing jsonrpc field");
  }
}

const JSONDocument& JSONRPCRequest::body() const {
  return body_;
}

std::string JSONRPCRequest::id() const {
  return body_.get("/id", "");
}

std::string JSONRPCRequest::method() const {
  return body_.get("/method", "");
}

} // namespace json
} // namsepace fnord

