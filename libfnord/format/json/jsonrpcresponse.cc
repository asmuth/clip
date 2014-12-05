/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/format/json/jsonrpcresponse.h"

namespace fnord {
namespace json {

JSONRPCResponse::JSONRPCResponse(
    JSONOutputStream&& output) :
    output_(std::move(output)) {}

void JSONRPCResponse::error(int code, const std::string& message) {
  output_.beginObject();

  output_.addObjectEntry("jsonrpc");
  output_.addString("2.0");
  output_.addComma();

  output_.addObjectEntry("error");
  output_.beginObject();
  output_.addObjectEntry("code");
  output_.addInteger(code);
  output_.addComma();
  output_.addObjectEntry("message");
  output_.addString(message);
  output_.endObject();
  output_.addComma();

  output_.addObjectEntry("id");
  if (id_.length() > 0) {
    output_.addString(id_);
  } else {
    output_.addNull();
  }

  output_.endObject();
}

void JSONRPCResponse::success(
    std::function<void (JSONOutputStream* output)> func) {
  output_.beginObject();

  output_.addObjectEntry("jsonrpc");
  output_.addString("2.0");
  output_.addComma();

  output_.addObjectEntry("id");
  if (id_.length() > 0) {
    output_.addString(id_);
  } else {
    output_.addNull();
  }
  output_.addComma();

  output_.addObjectEntry("result");
  func(&output_);

  output_.endObject();
}

void JSONRPCResponse::setID(const std::string& id) {
  id_ = id;
}

} // namespace json
} // namsepace fnord

