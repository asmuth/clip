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

void testMetricKeyGeneration() {
  fnordmetric::MetricDescription metric_description("mymetric", "kg", "blah");

  fnordmetric::MetricKey<
      fnordmetric::IntegerDimension,
      fnordmetric::IntegerDimension> metric_key(
      metric_description,
      fnordmetric::IntegerDimension("foo"),
      fnordmetric::IntegerDimension("bar"));

  printf("key: %s\n", metric_key.getKeyString().c_str());

  assert(metric_key.getKeyString() == "83d2f71c457206bf-Ia9f37ed7-I76b77d1a");
}

void testRecordSample() {

  fnordmetric::Agent agent("my_fnordmetric_agent",
      fnordmetric::FileBackend::openFile("/tmp/fm-test.db"));

  auto metric = agent.newMetric(
      fnordmetric::MetricDescription("my_metric", "kg", "blah blah"),
      fnordmetric::IntegerDimension("count"),
      fnordmetric::IntegerDimension("fnord"));

  metric->recordSample(123, 123.4f);
}
int main() {
  testMetricKeyGeneration();
  testRecordSample();
}
