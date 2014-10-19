/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_SAMPLEWRITER_H
#define _FNORDMETRIC_METRICDB_SAMPLEWRITER_H
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnordmetric {
namespace metricdb {
class SampleFieldIndex;

class SampleWriter {
public:
  static const size_t kInitialDataSize = 4096;

  SampleWriter(SampleFieldIndex* label_index);
  ~SampleWriter();

  void writeValue(uint64_t value);
  void writeLabel(const std::string& key, const std::string& value);

  void* data() const;
  size_t size() const;

protected:
  void append(void const* data, size_t size);

  SampleFieldIndex* label_index_;
  void* ptr_;
  size_t size_;
  size_t used_;
};

}
}

#endif
