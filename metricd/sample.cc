/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/sample.h>

namespace fnordmetric {

Sample::Sample(
    TimestampType time,
    double value) :
    time_(time),
    value_(value) {}

TimestampType Sample::getTime() {
  return time_;
}

double Sample::getValue() {
  return value_;
}

LabelledSample::LabelledSample(
    Sample sample,
    const LabelSet& labels) :
    sample_(sample),
    labels_(labels) {}

const Sample& LabelledSample::getSample() const {
  return sample_;
}

const LabelSet& LabelledSample::getLabels() const {
  return labels_;
}

} // namespace fnordmetric

