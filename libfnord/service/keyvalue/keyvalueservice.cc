/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/service/keyvalue/keyvalueservice.h"

namespace fnord {
namespace keyvalue_service {

KeyValueService::KeyValueService() {}

bool KeyValueService::get(const std::string& key, std::string* dst) {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  const auto& iter = data_.find(key);

  if (iter == data_.end()) {
    return false;
  } else {
    *dst = iter->second;
    return true;
  }
}

void KeyValueService::set(const std::string& key, const std::string& value) {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  data_[key] = value;
}

} // namespace keyvalue_service
} // namsepace fnord
