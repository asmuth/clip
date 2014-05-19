/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_METRIC_H
#define _FNORDMETRIC_METRIC_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include "dimension.h"
#include "serialize.h"

namespace fnordmetric {

class IStorageCursor;

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

  const std::string& getName() const {
    return name_;
  }

protected:

  std::string name_;
  std::string unit_;
  std::string description_;

};


class IMetric {
public:

  explicit IMetric(
      std::unique_ptr<IStorageCursor>&& cursor,
      const MetricDescription& description,
      const std::vector<IDimension>& dimensions) :
      cursor_(std::move(cursor)),
      description_(description),
      dimensions_(dimensions) {}

  virtual ~IMetric() {};

  const std::vector<IDimension>& getDimensions() const {
    return dimensions_;
  }

protected:

  void recordSample(const std::vector<uint8_t>& row) const;
  const std::unique_ptr<IStorageCursor> cursor_;
  const MetricDescription description_;
  const std::vector<IDimension> dimensions_;

};

class IMetricKey {
public:

  const std::string& getKeyString() const {
    return key_str_;
  }

protected:

  std::string key_str_;

};

template <typename... D>
class MetricKey : public IMetricKey {
public:

  MetricKey(
      const std::string& agent_name,
      const MetricDescription& description,
      D... dimensions) {
    const IDimension dims[] = {dimensions...};
    int num_dimensions = sizeof(dims) / sizeof(IDimension);

    key_str_.append(agent_name);
    key_str_.append("-");
    key_str_.append(description.getName());

    for (int i = 0; i < num_dimensions; ++i) {
      key_str_.append("-");
      //unpacked.push_back(packed[i]);
    }
  }

};

/**
 * Multidimensional metric
 */
template <typename... D>
class Metric : public IMetric {
public:

  explicit Metric(
      std::unique_ptr<IStorageCursor>&& cursor,
      const MetricDescription& description,
      const D... dimensions) :
      IMetric(std::move(cursor), description, unpackDimensions(dimensions...)) {}

  Metric(const Metric& copy) = delete;

  void recordSample(const typename D::ValueType... values) const {
    std::vector<uint8_t> sample;
    fnordmetric::serialize::toBytesV(&sample, values...);
    IMetric::recordSample(sample);
  }

protected:

 // FIXPAUL there must be some better way to do this...
  std::vector<IDimension> unpackDimensions(D... dimensions) {
    const IDimension packed[] = {dimensions...};
    std::vector<IDimension> unpacked;
    int num_dimensions = sizeof(packed) / sizeof(IDimension);

    for (int i = 0; i < num_dimensions; ++i) {
      unpacked.push_back(packed[i]);
    }

    return unpacked;
  };

};

}

#endif
