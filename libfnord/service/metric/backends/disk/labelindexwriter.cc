/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/service/metric/backends/disk/labelindexwriter.h>

namespace fnord {
namespace metric_service {
namespace disk_backend {

LabelIndexWriter::LabelIndexWriter(LabelIndex* index) {
  auto labels = index->labels();

  for (const auto& label : labels) {
    appendUInt32(label.size());
    appendString(label);
  }
}

}
}
}

