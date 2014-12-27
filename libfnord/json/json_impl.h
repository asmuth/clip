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
#include "fnord/base/datetime.h"
#include "fnord/json/jsonutil.h"
#include "fnord/json/jsonoutputstream.h"
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
    typename O,
    typename = typename std::enable_if<
        fnord::reflect::is_reflected<T>::value>::type>
void toJSON(const T& value, O* target) {
  JSONOutputProxy<O> proxy(value, target);
}

template <
    typename T,
    typename O,
    typename = typename std::enable_if<
        !fnord::reflect::is_reflected<T>::value>::type,
    typename = void>
void toJSON(const T& value, O* target) {
  toJSONImpl(value, target);
}

template <typename T>
std::string toJSONString(const T& value) {
  std::string json_str;
  JSONOutputStream json(
      static_cast<std::unique_ptr<io::OutputStream>>(
          io::StringOutputStream::fromString(&json_str)));

  toJSON(value, &json);
  return json_str;
}

template <typename T>
JSONObject toJSON(const T& value) {
  JSONObject obj;
  toJSON(value, &obj);
  return obj;
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

template <typename OutputType>
template <typename T>
JSONOutputProxy<OutputType>::JSONOutputProxy(
    const T& instance,
    OutputType* target) :
    target_(target) {
  target_->emplace_back(json::JSON_OBJECT_BEGIN);
  fnord::reflect::MetaClass<T>::serialize(instance, this);
  target_->emplace_back(json::JSON_OBJECT_END);
}

template <typename OutputType>
template <typename T>
void JSONOutputProxy<OutputType>::putProperty(
    uint32_t id,
    const std::string& name,
    const T& value) {
  target_->emplace_back(json::JSON_STRING, name);
  toJSON(value, target_);
}

template <typename T, typename O>
void toJSONImpl(const std::vector<T>& value, O* target) {
  target->emplace_back(json::JSON_ARRAY_BEGIN);

  for (const auto& e : value) {
    toJSON(e, target);
  }

  target->emplace_back(json::JSON_ARRAY_END);
}

template <typename O>
void toJSONImpl(const std::string& str, O* target) {
  target->emplace_back(json::JSON_STRING, str);
}

template <typename O>
void toJSONImpl(const char* const& str, O* target) {
  target->emplace_back(json::JSON_STRING, str);
}

template <typename O>
void toJSONImpl(unsigned long long const& val, O* target) {
  target->emplace_back(json::JSON_NUMBER, StringUtil::toString(val));
}

template <typename O>
void toJSONImpl(int const& val, O* target) {
  target->emplace_back(json::JSON_NUMBER, StringUtil::toString(val));
}

template <typename O>
void toJSONImpl(const fnord::DateTime& val, O* target) {
  toJSONImpl(static_cast<uint64_t>(val), target);
}

}
}
#endif
