/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_JSONARGUMENTLIST_H
#define _FNORD_JSON_JSONARGUMENTLIST_H
#include <functional>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <time.h>
#include "fnord-json/json.h"

namespace fnord {
namespace json {

class JSONArgumentList {
public:

  JSONArgumentList(const JSONObject& obj);

protected:
  const JSONObject& obj_;
};

} // namespace json
} // namsepace fnord
#endif
