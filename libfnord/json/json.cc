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
#include "fnord/base/inspect.h"
#include "fnord/base/datetime.h"
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
  return parseJSON(&json);
}

JSONObject parseJSON(const fnord::Buffer& json_buf) {
  JSONInputStream json(io::BufferInputStream::fromBuffer(&json_buf));
  return parseJSON(&json);
}

JSONObject parseJSON(JSONInputStream* json) {
  JSONObject obj;
  std::stack<size_t> stack;

  kTokenType token;
  std::string token_str;
  while (json->readNextToken(&token, &token_str)) {
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

template <>
int fromJSON(
    std::vector<JSONToken>::const_iterator begin,
    std::vector<JSONToken>::const_iterator end) {
  if (begin == end) {
    RAISE(kIndexError);
  }

  switch (begin->type) {
    case JSON_STRING:
    case JSON_NUMBER:
      try {
        return std::stoi(begin->data);
      } catch (std::exception& e) {
        /* fallthrough */
      }

    default:
      RAISEF(
          kParseError,
          "can't convert $0 ($1) to integer",
          begin->type,
          begin->data);

  }
}

template <>
JSONObject toJSONImpl(const std::string& str) {
  return JSONObject { JSONToken(json::JSON_STRING, str) };
}

template <>
JSONObject toJSONImpl(const char* const& str) {
  return JSONObject { JSONToken(json::JSON_STRING, str) };
}

template <>
JSONObject toJSONImpl(unsigned long long const& val) {
  return JSONObject { JSONToken(json::JSON_NUMBER, StringUtil::toString(val)) };
}

template <>
JSONObject toJSONImpl(int const& val) {
  return JSONObject { JSONToken(json::JSON_NUMBER, StringUtil::toString(val)) };
}

template <>
JSONObject toJSONImpl(const fnord::DateTime& val) {
  return toJSON(static_cast<uint64_t>(val));
}

} // namespace json

template <>
std::string StringUtil::toString(json::kTokenType type) {
  switch (type) {
    case json::JSON_OBJECT_BEGIN: return "JSON_OBJECT_BEGIN";
    case json::JSON_OBJECT_END: return "JSON_OBJECT_END";
    case json::JSON_ARRAY_BEGIN: return "JSON_ARRAY_BEGIN";
    case json::JSON_ARRAY_END: return "JSON_ARRAY_END";
    case json::JSON_STRING: return "JSON_STRING";
    case json::JSON_NUMBER: return "JSON_NUMBER";
    case json::JSON_TRUE: return "JSON_TRUE";
    case json::JSON_FALSE: return "JSON_FALSE";
    case json::JSON_NULL: return "JSON_NULL";
  }
}

template <>
std::string fnord::inspect(const json::JSONToken& token) {
  return StringUtil::format(
      "$0:$1",
      fnord::StringUtil::toString(token.type),
      token.data);
}

} // namespace fnord
