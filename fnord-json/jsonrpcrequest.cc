/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/exception.h"
#include "fnord/json/jsonrpcrequest.h"
#include "fnord/json/jsonutil.h"

namespace fnord {
namespace json {

JSONRPCRequest::JSONRPCRequest(
    JSONObject&& body) :
    body_(std::move(body)) {
  auto v_iter = JSONUtil::objectLookup(body_.begin(), body_.end(), "jsonrpc");
  if (v_iter == body_.end()) {
    RAISE(kRuntimeError, "invalid JSONRPC 2.0 request, missing jsonrpc field");
  }

  auto v = fromJSON<std::string>(v_iter, body_.end());
  if (v != "2.0") {
    RAISEF(kRuntimeError, "invalid JSONRPC 2.0 request version: $0", v);
  }

  auto id_iter = JSONUtil::objectLookup(body_.begin(), body_.end(), "id");
  if (id_iter != body_.end()) {
    id_ = fromJSON<std::string>(id_iter, body_.end());
  }

  auto m_iter = JSONUtil::objectLookup(body_.begin(), body_.end(), "method");
  if (m_iter == body_.end()) {
    RAISE(kRuntimeError, "invalid JSONRPC 2.0 request, missing method field");
  } else {
    method_ = fromJSON<std::string>(m_iter, body_.end());
  }

  params_ = JSONUtil::objectLookup(body_.begin(), body_.end(), "params");
  if (params_ == body_.end()) {
    RAISE(kRuntimeError, "invalid JSONRPC 2.0 request, missing params");
  }
}

JSONObject::const_iterator JSONRPCRequest::paramsBegin() const {
  return params_;
}

JSONObject::const_iterator JSONRPCRequest::paramsEnd() const {
  return body_.end();
}

const std::string& JSONRPCRequest::id() const {
  return id_;
}

const std::string& JSONRPCRequest::method() const {
  return method_;
}

} // namespace json
} // namsepace fnord

