/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_SAMPLEFIELDINDEX_H
#define _FNORDMETRIC_METRICDB_SAMPLEFIELDINDEX_H
#include <fnordmetric/sstable/index.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace metricdb  {

// FIXPAUL rename to SampleLabelIndex
class SampleFieldIndex : public fnord::sstable::Index {
public:
  static const uint32_t kIndexType = 0xf8f8;

  static SampleFieldIndex* makeIndex();

  SampleFieldIndex();

  void addRow(
      size_t body_offset,
      void const* key,
      size_t key_size,
      void const* data,
      size_t data_size) const override;

};


}
}

#endif
