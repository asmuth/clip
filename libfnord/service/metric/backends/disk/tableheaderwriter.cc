/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/service/metric/backends/disk/tableheaderwriter.h>

namespace fnord {
namespace metric_service {
namespace disk_backend {

TableHeaderWriter::TableHeaderWriter(
    const std::string& metric_key,
    uint64_t generation,
    const std::vector<uint64_t>& parents) {
  appendUInt32(metric_key.size());
  appendString(metric_key);
  appendUInt64(generation);
  appendUInt32(parents.size());
  for (const auto parent : parents) {
    appendUInt64(parent);
  }
}

}
}
}
