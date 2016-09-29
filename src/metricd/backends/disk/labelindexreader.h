/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_LABELINDEXREADER_H
#define _FNORDMETRIC_METRICDB_LABELINDEXREADER_H
#include <metricd/backends/disk/labelindex.h>
#include <fnord/util/binarymessagereader.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace fnord {
namespace metric_service {
namespace disk_backend {

class LabelIndexReader : public fnord::util::BinaryMessageReader {
public:
  LabelIndexReader(
      void* data,
      size_t size);

  void readIndex(LabelIndex* token_index);

};

}
}
}

#endif
