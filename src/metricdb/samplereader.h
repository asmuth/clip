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
class TokenIndex;

class AbstractSampleReader : public fnord::util::BinaryMessageReader {
public:
  AbstractSampleReader(
      void* data,
      size_t size,
      TokenIndex* token_index);

  std::vector<std::pair<std::string, std::string>> labels();

protected:
  std::string readToken();

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

#include "samplereader_impl.h"
#endif
