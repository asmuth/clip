/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_METRIC_H
#define _FNORDMETRIC_METRIC_H

#include <stdint.h>
#include <string>
#include <vector>
#include "dimension.h"
#include "sample.h"

namespace fnordmetric {

class MetricDescription {
public:

  MetricDescription(const std::string& name) : name_(name) {}

  MetricDescription(const std::string& name, const std::string& unit) :
      name_(name),
      unit_(unit) {}

  MetricDescription(
      const std::string& name,
      const std::string& unit,
      const std::string& description) :
      name_(name),
      unit_(unit),
      description_(description) {}

protected:

  std::string name_;
  std::string unit_;
  std::string description_;

};


class IMetric {
public:

  explicit IMetric(
      Agent* agent,
      const MetricDescription& description) :
      agent_(agent),
      description_(description) {}

  virtual ~IMetric() {};

protected:

  const Agent* agent_;
  const MetricDescription description_;
  std::vector<IDimension> dimensions_;

};

/**
 * Multidimensional metric
 */
template <typename... D>
class Metric : public IMetric {
public:

  explicit Metric(
      Agent* agent,
      const MetricDescription& description,
      const D... dimensions) :
      IMetric(agent, description) {
    const IDimension dims[] = {dimensions...};
    int num_dimensions = sizeof(dims) / sizeof(IDimension);

    for (int i = 0; i < num_dimensions; ++i) {
      dimensions_.push_back(dims[i]);
    }
  };

  Metric(const Metric& copy) = delete;

  void recordSample(const typename D::ValueType... values) const {
    Sample sample;
    sample.addValues(values...);
  }

};

}

#endif
