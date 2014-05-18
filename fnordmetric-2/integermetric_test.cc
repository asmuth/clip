/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <cstdio>
#include "metric.h"
#include "agent.h"

int main() {
  fnordmetric::Agent agent("my_fnordmetric_agent");

  auto metric = agent.newMetric(
      fnordmetric::MetricDescription("my_metric", "kg", "blah blah"),
      fnordmetric::IntegerDimension("count"),
      fnordmetric::IntegerDimension("fnord"));

  metric->recordSample(123, 123.4f);
}
