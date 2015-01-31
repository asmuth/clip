/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_KEYVALLUE_SERVICE_H
#define _FNORD_KEYVALLUE_SERVICE_H
#include <mutex>
#include <stdlib.h>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace fnord {
namespace keyvalue_service {

class KeyValueService {
public:

  KeyValueService();

  bool get(const std::string& key, std::string* dst);
  std::vector<std::string> mget(const std::vector<std::string>& keys);

  void set(const std::string& key, const std::string& value);

protected:
  std::unordered_map<std::string, std::string> data_;
  mutable std::mutex mutex_;
};

} // namespace keyvalue_service
} // namsepace fnord
#endif
