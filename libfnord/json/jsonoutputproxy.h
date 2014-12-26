/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSONOUTPUTPROXY_H
#define _FNORD_JSONOUTPUTPROXY_H
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord/base/buffer.h"

namespace fnord {
namespace json {

template <typename T>
class JSONOutputProxy {
public:

  JSONOutputProxy(const T& value) {
    reflect::MetaClass<T>::reflectProperties(this);
  }

  const JSONObject& object() const {
    return obj_;
  }

protected:
  JSONObject obj_;
};

}
}

#endif
