/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_H
#define _FNORD_JSON_H
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord/base/buffer.h"
#include "fnord/reflect/reflect.h"

namespace fnord {
namespace json {
class JSONOutputStream;
class JSONInputStream;

enum kTokenType {
  JSON_OBJECT_BEGIN,
  JSON_OBJECT_END,
  JSON_ARRAY_BEGIN,
  JSON_ARRAY_END,
  JSON_STRING,
  JSON_NUMBER,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NULL
};

struct JSONToken {
  JSONToken(kTokenType _type, const std::string& _data);
  JSONToken(kTokenType _type);
  kTokenType type;
  std::string data;
  uint32_t size;
};

typedef std::vector<JSONToken> JSONObject;

template <typename T>
JSONObject toJSONImpl(const std::vector<T>& value);

template <typename T>
JSONObject toJSONImpl(const T& value);

struct JSONOutputProxy {
public:
  template <typename T>
  JSONOutputProxy(const T& instance);

  template <typename T>
  void putProperty(uint32_t id, const std::string& name, const T& value);

  JSONObject object;
};

template <typename T>
std::string toJSONString(const T& value);

template <typename T>
T fromJSON(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end);

template <typename T>
T fromJSON(const std::string& json_str);

JSONObject parseJSON(const std::string& json_str);
JSONObject parseJSON(const fnord::Buffer& json_buf);
JSONObject parseJSON(JSONInputStream* json);

}
}

#include "json_impl.h"
#endif
