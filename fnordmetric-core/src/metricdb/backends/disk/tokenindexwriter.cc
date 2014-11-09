/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/backends/disk/tokenindexwriter.h>

namespace fnordmetric {
namespace metricdb {
namespace disk_backend {

TokenIndexWriter::TokenIndexWriter(TokenIndex* index) {
  auto token_ids = index->tokenIDs();

  for (const auto& pair : token_ids) {
    appendUInt32(pair.second);
    appendUInt32(pair.first.size());
    appendString(pair.first);
  }
}

}
}
}

