/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/tokenindex.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace metricdb  {

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


}
}

