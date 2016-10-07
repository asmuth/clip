/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/metricdb/backends/disk/tokenindex.h>
#include <metricd/util/runtimeexception.h>

namespace fnordmetric {
namespace metricdb {
namespace disk_backend {

TokenIndex::TokenIndex() : max_token_id_(kMinTokenID) {}

uint32_t TokenIndex::findToken(const std::string& key) const {
  std::lock_guard<std::mutex> lock_holder(mutex_);

  auto iter = token_ids_.find(key);
  if (iter == token_ids_.end()) {
    return 0;
  } else {
    return iter->second;
  }
}

uint32_t TokenIndex::addToken(const std::string& key) {
  std::lock_guard<std::mutex> lock_holder(mutex_);

  auto iter = token_ids_.find(key);
  if (iter != token_ids_.end()) {
    RAISE(kIllegalStateError, "label already exists in index");
  }

  auto new_id = ++max_token_id_;
  token_ids_.emplace(key, new_id);

  return new_id;
}

void TokenIndex::addToken(const std::string& key, uint32_t id) {
  std::lock_guard<std::mutex> lock_holder(mutex_);

  auto iter = token_ids_.find(key);
  if (iter == token_ids_.end()) {
    token_ids_.emplace(key, id);
  } else if (iter->second != id) {
    RAISE(
        kIllegalStateError,
        "conflicting token definitions for token '%s'\n",
        key.c_str());
  }

  if (id > max_token_id_) {
    max_token_id_ = id;
  }
}

std::string TokenIndex::resolveToken(uint32_t token_id) const {
  std::lock_guard<std::mutex> lock_holder(mutex_);

  // FIXPAUL!
  for (const auto& pair : token_ids_) {
    if (pair.second == token_id) {
      return pair.first;
    }
  }

  RAISE(kIndexError, "token not found, %i", (int) token_id);
}

std::unordered_map<std::string, uint32_t> TokenIndex::tokenIDs() const {
  std::unordered_map<std::string, uint32_t> copy;

  {
    std::lock_guard<std::mutex> lock_holder(mutex_);
    copy = token_ids_;
  }

  return copy;
}

}
}
}

