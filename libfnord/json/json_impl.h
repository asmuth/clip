/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_IMPL_H
#define _FNORD_JSON_IMPL_H
#include "fnord/base/exception.h"
#include "fnord/json/jsonutil.h"
#include "fnord/reflect/reflect.h"

namespace fnord {
namespace json {

template <
    typename T,
    typename = typename std::enable_if<
        fnord::reflect::is_reflected<T>::value>::type>
T fromJSON(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) {
  return JSONInputProxy<T>(begin, end).value;
}

template <
    typename T,
    typename = typename std::enable_if<
        !fnord::reflect::is_reflected<T>::value>::type,
    typename = void>
T fromJSON(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) {
  return fromJSONImpl<T>(begin, end);
}

template <typename T>
T fromJSON(const std::string& json_str) {
  return fromJSON<T>(parseJSON(json_str));
}

template <typename T>
T fromJSON(const JSONObject& json_obj) {
  return fromJSON<T>(json_obj.begin(), json_obj.end());
}

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
std::string toJSONString(const T& value) {
  std::string json_str;
  auto tokens = toJSON(value);
  return json_str;
}

template <typename T>
JSONInputProxy<T>::JSONInputProxy(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) :
    obj_begin(begin),
    obj_end(end),
    value(fnord::reflect::MetaClass<T>::unserialize(this)) {}

template <typename T>
template <typename PropertyType>
PropertyType JSONInputProxy<T>::getProperty(
    uint32_t id,
    const std::string& name) {
  auto iter = JSONUtil::objectLookup(obj_begin, obj_end, name);

  if (iter == obj_end) {
    RAISEF(kIndexError, "no such element: $0", name);
  }

  return fromJSON<PropertyType>(iter, obj_end);
}

template <typename T>
JSONOutputProxy::JSONOutputProxy(const T& instance) {
  object.emplace_back(json::JSON_OBJECT_BEGIN);
  fnord::reflect::MetaClass<T>::serialize(instance, this);
  object.emplace_back(json::JSON_OBJECT_END);
  object[0].size = object.size();
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
  object[0].size = object.size();
  return object;
}

}
}
#endif
