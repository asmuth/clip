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
#include <fnordmetric/metricdb/samplereader.h>
#include <fnordmetric/util/ieee754.h>

namespace fnordmetric {
namespace metricdb {

SampleReader::SampleReader(
    void* data,
    size_t size,
    TokenIndex* token_index) :
    fnord::util::BinaryMessageReader(data, size),
    token_index_(token_index) {}

std::vector<std::pair<std::string, std::string>> SampleReader::labels() {
  std::vector<std::pair<std::string, std::string>> labels;
  return labels;
}

template <> double SampleReader::value<double>() {
  return fnord::util::IEEE754::fromBytes(*readUInt64());
}

}
}

