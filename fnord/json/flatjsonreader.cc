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
#include <string>
#include "fnord/base/exception.h"
#include "fnord/base/inspect.h"
#include "fnord/json/flatjsonreader.h"

namespace fnord {
namespace json {

FlatJSONReader::FlatJSONReader(
    JSONInputStream&& json) :
    json_(std::move(json)) {}

void FlatJSONReader::read(
    std::function<bool (const JSONPointer&, const std::string&)> func) {
  kTokenType token;
  std::string token_str;
  JSONPointer path;

  if (!json_.readNextToken(&token, &token_str)) {
    RAISE(kRuntimeError, "invalid JSON. unexpected end of stream");
  }

  switch (token) {
    case JSON_OBJECT_BEGIN:
      readObject(func, &path);
      return;

    case JSON_ARRAY_BEGIN:
      readArray(func, &path);
      return;

    default:
      RAISE(kRuntimeError, "invalid JSON. root node must be OBJECT or ARRAY");
  }
}

void FlatJSONReader::readObject(
    std::function<bool (const JSONPointer&, const std::string&)> func,
    JSONPointer* path) {
  for (;;) {
    kTokenType key_token;
    std::string key_str;

    if (!json_.readNextToken(&key_token, &key_str)) {
      RAISE(kRuntimeError, "invalid JSON. unexpected end of stream");
    }

    switch (key_token) {
      case JSON_OBJECT_END:
        return;

      case JSON_STRING:
      case JSON_NUMBER:
        path->push(key_str);
        break;

      case JSON_TRUE:
        path->push("true");
        break;

      case JSON_FALSE:
        path->push("false");
        break;

      case JSON_NULL:
        path->push("null");
        break;

      default:
        RAISE(
            kRuntimeError,
            "invalid JSON. object key must be of type string, number, boolean "
            "or null");
    }

    kTokenType value_token;
    std::string value_str;

    if (!json_.readNextToken(&value_token, &value_str)) {
      RAISE(kRuntimeError, "invalid JSON. unexpected end of stream");
    }

    switch (value_token) {
      case JSON_OBJECT_BEGIN:
        readObject(func, path);
        break;

      case JSON_ARRAY_BEGIN:
        readArray(func, path);
        break;

      case JSON_STRING:
      case JSON_NUMBER:
      case JSON_TRUE:
      case JSON_FALSE:
      case JSON_NULL:
        func(*path, value_str);
        break;

      default:
        RAISE(
            kRuntimeError,
            "invalid JSON. object key must be of type string, number, boolean "
            "or null");
    }

    path->pop();
  }
}

void FlatJSONReader::readArray(
    std::function<bool (const JSONPointer&, const std::string&)> func,
    JSONPointer* path) {
  for (int i = 0; ; i++) {
    kTokenType elem_token;
    std::string elem_str;

    if (!json_.readNextToken(&elem_token, &elem_str)) {
      RAISE(kRuntimeError, "invalid JSON. unexpected end of stream");
    }

    path->push(std::to_string(i));

    switch (elem_token) {
      case JSON_ARRAY_END:
        return;

      case JSON_OBJECT_BEGIN:
        readObject(func, path);
        break;

      case JSON_ARRAY_BEGIN:
        readArray(func, path);
        break;

      case JSON_STRING:
      case JSON_NUMBER:
        func(*path, elem_str);
        break;

      case JSON_TRUE:
        func(*path, "true");
        break;

      case JSON_FALSE:
        func(*path, "false");
        break;

      case JSON_NULL:
        func(*path, "null");
        break;

      default:
        RAISE(
            kRuntimeError,
            "invalid JSON. unexpected token");
    }

    path->pop();
  }
}

} // namespace json
} // namsepace fnord

