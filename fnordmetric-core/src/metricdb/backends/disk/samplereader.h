/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_SAMPLEREADER_H
#define _FNORDMETRIC_METRICDB_SAMPLEREADER_H
#include <fnordmetric/util/binarymessagereader.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnordmetric {
namespace metricdb {
namespace disk_backend{

class TokenIndex;

class AbstractSampleReader : public fnord::util::BinaryMessageReader {
public:
  AbstractSampleReader(
      void* data,
      size_t size,
      TokenIndex* token_index);

  const std::vector<std::pair<std::string, std::string>>& labels();
  std::vector<std::pair<uint32_t, std::string>> tokenDefinitions();

protected:
  std::string readToken();
  size_t label_offset_;
  TokenIndex* token_index_;
  std::vector<std::pair<std::string, std::string>> labels_;
  bool labels_read_;
};

template <typename T>
class SampleReader : public AbstractSampleReader {
public:

  SampleReader(
      void* data,
      size_t size,
      TokenIndex* token_index);

  const T& value();

protected:
  T readValue();
  T value_;
};

}
}
}

#include "samplereader_impl.h"
#endif
