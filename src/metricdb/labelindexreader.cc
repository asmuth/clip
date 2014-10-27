/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/labelindexreader.h>

namespace fnordmetric {
namespace metricdb {

LabelIndexReader::LabelIndexReader(
    void* data,
    size_t size) :
    fnord::util::BinaryMessageReader(data, size) {}

void LabelIndexReader::readIndex(LabelIndex* index) {
  while (pos_ < size_) {
    auto str_size = *readUInt32();
    auto str = std::string(readString(str_size), str_size);
    index->addLabel(str);
  }
}

}
}

