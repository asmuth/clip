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
#pragma once
#include <stdlib.h>
#include <string>
#include <map>

namespace fnordmetric {

using TimestampType = uint64_t;
using LabelSet = std::map<std::string, std::string>; // FIXME should be a flat map

class Sample {
public:

  Sample(
      TimestampType time,
      double value);

  TimestampType getTime() const;
  double getValue() const;

protected:
  const TimestampType time_;
  const double value_;
};

class LabelledSample {
public:

  LabelledSample(Sample sample, const LabelSet& labels);

  const Sample& getSample() const;
  const LabelSet& getLabels() const;
  const std::string& getSeriesName() const;

protected:
  const Sample sample_;
  const LabelSet labels_;
  const std::string series_name_;
};

} // namespace fnordmetric

