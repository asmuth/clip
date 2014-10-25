/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/tableheaderreader.h>

namespace fnordmetric {
namespace metricdb {

TableHeaderReader::TableHeaderReader(
    void* data,
    size_t size) :
    fnord::util::BinaryMessageReader(data, size) {
  size_t metric_key_size = *readUInt32();
  metric_key_ = std::string(readString(metric_key_size), metric_key_size);
}

const std::string& TableHeaderReader::metricKey() const {
  return metric_key_;
}

}
}

