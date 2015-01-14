/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSONUTIL_H
#define _FNORD_JSONUTIL_H
#include "fnord/json/jsontypes.h"

namespace fnord {
namespace json {

class JSONUtil {
public:

  static JSONObject::const_iterator objectLookup(
      JSONObject::const_iterator begin,
      JSONObject::const_iterator end,
      const std::string& key);

  static size_t arrayLength(
      JSONObject::const_iterator begin,
      JSONObject::const_iterator end);

  static JSONObject::const_iterator arrayLookup(
      JSONObject::const_iterator begin,
      JSONObject::const_iterator end,
      size_t index);
};

}
}

#endif
