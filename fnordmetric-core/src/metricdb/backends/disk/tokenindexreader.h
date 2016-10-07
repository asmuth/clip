/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_TOKENINDEXREADER_H
#define _FNORDMETRIC_METRICDB_TOKENINDEXREADER_H
#include <metricd/metricdb/backends/disk/tokenindex.h>
#include <metricd/util/binarymessagereader.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace metricdb {
namespace disk_backend {

class TokenIndexReader : public fnord::util::BinaryMessageReader {
public:
  TokenIndexReader(
      void* data,
      size_t size);

  void readIndex(TokenIndex* token_index);

};

}
}
}

#endif
