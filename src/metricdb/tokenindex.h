/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_TOKENINDEX_H
#define _FNORDMETRIC_METRICDB_TOKENINDEX_H
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace fnordmetric {
namespace metricdb  {

class TokenIndex {
public:
  static const int kMinTokenID = 0xf0000000;

  TokenIndex();

  uint32_t findToken(const std::string& key) const;
  uint32_t addToken(const std::string& key);

protected:
  std::unordered_map<std::string, uint32_t> token_ids_;
  uint32_t max_token_id_;
  mutable std::mutex mutex_;
};


}
}

#endif
