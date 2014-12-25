/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stack>
#include <string>
#include "fnord/json/json.h"
#include "fnord/json/jsoninputstream.h"

namespace fnord {
namespace json {

JSONToken::JSONToken(
    kTokenType _type,
    const std::string& _data) :
    type(_type),
    data(_data),
    size(1) {}

JSONToken::JSONToken(
    kTokenType _type) :
    type(_type),
    size(1) {}

JSONObject parseJSON(const std::string& json_str) {
  JSONInputStream json(io::StringInputStream::fromString(json_str));

  JSONObject obj;
  std::stack<size_t> stack;

  kTokenType token;
  std::string token_str;
  while (json.readNextToken(&token, &token_str)) {
    switch (token) {
      case JSON_ARRAY_BEGIN:
      case JSON_OBJECT_BEGIN:
        obj.emplace_back(token);
        stack.push(obj.size() - 1);
        break;

      case JSON_ARRAY_END:
      case JSON_OBJECT_END: {
        obj.emplace_back(token);
        obj[stack.top()].size = obj.size() - stack.top();
        stack.pop();
      }

      case JSON_STRING:
      case JSON_NUMBER:
        obj.emplace_back(token, token_str);
        break;

      case JSON_TRUE:
      case JSON_FALSE:
      case JSON_NULL:
        obj.emplace_back(token);
        break;
    }

    token_str.clear();
  }

  if (!stack.empty()) {
    RAISE(kRuntimeError, "invalid JSON. unexpected end of stream");
  }

  return obj;
}

template <>
std::string fromJSON(
    std::vector<JSONToken>::const_iterator begin,
    std::vector<JSONToken>::const_iterator end) {
  if (begin == end) {
    RAISE(kIndexError);
  }

  switch (begin->type) {
    case JSON_STRING:
    case JSON_NUMBER:
      return begin->data;

    case JSON_TRUE:
      return "true";

    case JSON_FALSE:
      return "false";

    case JSON_NULL:
      return "null";

    default:
      RAISEF(kParseError, "can't convert $0 to string", begin->type);

  }
}

} // namespace json

template <>
std::string StringUtil::toString(json::kTokenType type) {
  return "<json token>";
}

} // namespace fnord
