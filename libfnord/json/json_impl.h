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
#include "fnord/reflect/reflect.h"

namespace fnord {
namespace json {

template <
    typename T,
    typename = typename std::enable_if<
        fnord::reflect::is_reflected<T>::value>::type>
JSONObject toJSON(const T& value) {
  return JSONOutputProxy(value).object;
}

template <
    typename T,
    typename = typename std::enable_if<
        !fnord::reflect::is_reflected<T>::value>::type,
    typename = void>
JSONObject toJSON(const T& value) {
  return toJSONImpl(value);
}

template <typename T>
T fromJSON(const std::string& json_str) {
  auto tokens = parseJSON(json_str);
  return fromJSON<T>(tokens.begin(), tokens.end());
}

template <typename T>
std::string toJSONString(const T& value) {
  std::string json_str;
  auto tokens = toJSON(value);
  return json_str;
}

template <typename T>
JSONOutputProxy::JSONOutputProxy(const T& instance) {
  object.emplace_back(json::JSON_OBJECT_BEGIN);
  fnord::reflect::MetaClass<T>::serialize(instance, this);
  object.emplace_back(json::JSON_OBJECT_END);
}

template <typename T>
void JSONOutputProxy::putProperty(
    uint32_t id,
    const std::string& name,
    const T& value) {
  object.emplace_back(json::JSON_STRING, name);
  const auto& val = toJSON(value);
  object.insert(object.end(), val.begin(), val.end());
}

template <typename T>
JSONObject toJSONImpl(const std::vector<T>& value) {
  JSONObject object;
  object.emplace_back(json::JSON_ARRAY_BEGIN);

  for (const auto& e : value) {
    const auto& val = toJSON(e);
    object.insert(object.end(), val.begin(), val.end());
  }

  object.emplace_back(json::JSON_ARRAY_END);
  return object;
}

}
}
