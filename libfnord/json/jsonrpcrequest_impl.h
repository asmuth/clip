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

template <typename T>
T JSONRPCRequest::getArg(size_t index, const std::string& name) const {
  auto begin = paramsBegin();
  auto end = paramsEnd();

  switch (begin->type) {
    case json::JSON_OBJECT_BEGIN: {
      auto iter = JSONUtil::objectLookup(begin, end, name);
      if (iter == end) {
        RAISEF(kIndexError, "missing argument: $0", name);
      }

      return fromJSON<T>(iter, end);
    }

    case json::JSON_ARRAY_BEGIN: {
      JSONObject::const_iterator iter;
      try {
        iter = JSONUtil::arrayLookup(begin, end, index);
      } catch (const std::exception& e) {
        RAISEF(kIndexError, "missing argument: $0", name);
      }

      return fromJSON<T>(iter, end);
    }

    default:
      RAISE(kParseError, "params type must be array or object");

  }
}

} // namespace json
} // namsepace fnord
