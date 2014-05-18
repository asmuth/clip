/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "metric.h"
#include "agent.h"
#include "filebackend.h"

int main() {
  auto backend = fnordmetric::FileBackend::openFile("/tmp/fm-test.db");
  assert(backend.get() != nullptr);

  fnordmetric::Agent agent("my_fnordmetric_agent", std::move(backend));

  auto metric = agent.newMetric(
      fnordmetric::MetricDescription("my_metric", "kg", "blah blah"),
      fnordmetric::IntegerDimension("count"),
      fnordmetric::IntegerDimension("fnord"));

  metric->recordSample(123, 123.4f);
}
