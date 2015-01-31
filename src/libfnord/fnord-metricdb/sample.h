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
#include <stdlib.h>
#include <string>
#include <vector>
#include <fnord-base/datetime.h>

using fnord::DateTime;

namespace fnord {
namespace metric_service {

class Sample {
public:
  Sample(
      const DateTime& time,
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels);

  const DateTime& time();
  double value();
  const std::vector<std::pair<std::string, std::string>>& labels();

protected:
  const DateTime& time_;
  double value_;
  const std::vector<std::pair<std::string, std::string>>& labels_;
};

}
}
#endif
