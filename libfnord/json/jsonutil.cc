/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/json/jsonutil.h"

namespace fnord {
namespace json {

JSONObject::const_iterator JSONUtil::objectLookup(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  if (begin->type != json::JSON_OBJECT_BEGIN) {
    RAISEF(kParseError, "expected JSON_OBJECT_BEGIN, got: $0", begin->type);
  }

  auto oend = begin + begin->size;
  if (oend > end) {
    RAISE(kIndexError);
  }

  for (++begin; begin != oend;) {
    switch (begin->type) {
      case json::JSON_STRING:
        if (begin->data == key) {
          return ++begin;
        }

      default:
        break;
    }

    if (++begin == oend) {
      break;
    } else {
      begin += begin->size;
    }
  }

  return end;
}

size_t JSONUtil::arrayLength(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) {
  if (begin->type != json::JSON_ARRAY_BEGIN) {
    RAISEF(kParseError, "expected JSON_OBJECT_BEGIN, got: $0", begin->type);
  }

  auto aend = begin + begin->size - 1;
  if (aend > end) {
    RAISE(kIndexError);
  }

  size_t length = 0;
  for (++begin; begin < aend; begin += begin->size) {
    length++;
  }

  return length;
}

JSONObject::const_iterator JSONUtil::arrayLookup(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    size_t index) {
  if (begin->type != json::JSON_ARRAY_BEGIN) {
    RAISEF(kParseError, "expected JSON_OBJECT_BEGIN, got: $0", begin->type);
  }

  auto aend = begin + begin->size - 1;
  if (aend > end || begin == aend) {
    RAISE(kIndexError);
  }

  size_t idx = 0;
  for (++begin; begin < aend; begin += begin->size) {
    if (idx++ == index) {
      return begin;
    }
  }

  RAISEF(kIndexError, "invalid index: $0", index);
  return end;
}


}
}

