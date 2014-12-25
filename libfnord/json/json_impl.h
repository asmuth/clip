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

namespace fnord {
namespace json {

template <typename T>
T fromJSON(const std::string& json_str) {
  auto tokens = parseJSON(json_str);
  return fromJSON<T>(tokens.begin(), tokens.end());
}

}
}
