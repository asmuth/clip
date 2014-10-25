/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_TABLEHEADERREADER_H
#define _FNORDMETRIC_METRICDB_TABLEHEADERREADER_H
#include <fnordmetric/util/binarymessagereader.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace metricdb {

class TableHeaderReader : public fnord::util::BinaryMessageReader {
public:
  TableHeaderReader(
      void* data,
      size_t size);

  const std::string& metricKey() const;

  const uint64_t generation() const;
  const std::vector<uint64_t>& parents() const;

protected:
  std::string metric_key_;
  uint64_t generation_;
  std::vector<uint64_t> parents_;
};

}
}

#endif
