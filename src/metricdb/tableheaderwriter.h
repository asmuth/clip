/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_TABLEHEADERWRITER_H
#define _FNORDMETRIC_METRICDB_TABLEHEADERWRITER_H
#include <fnordmetric/util/binarymessagewriter.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnordmetric {
namespace metricdb {

class TableHeaderWriter : public fnord::util::BinaryMessageWriter {
public:
  TableHeaderWriter(
      const std::string& metric_key);
};

}
}

#endif
