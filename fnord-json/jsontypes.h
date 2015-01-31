/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSONTYPES_H
#define _FNORD_JSONTYPES_H
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord-base/buffer.h"
#include "fnord-base/reflect/reflect.h"
#include "fnord-base/traits.h"

namespace fnord {
namespace json {

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

} // namespace json

template <>
struct TypeIsVector<json::JSONObject> {
  static const bool value = false;
};

} // namespace fnord

#endif
