/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_SAMPLE_H_
#define _FNORDMETRIC_METRICDB_SAMPLE_H_
#include <fnordmetric/util/datetime.h>
#include <stdlib.h>
#include <string>
#include <vector>

using fnord::util::DateTime;

namespace fnordmetric {
namespace metricdb {

class Sample {
public:
  const DateTime& time();
  double value();
  const std::vector<std::pair<std::string, std::string>>& labels();
};

}
}
#endif
