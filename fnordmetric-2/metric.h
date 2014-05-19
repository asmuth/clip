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
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include "dimension.h"
#include "serialize.h"
#include "fnv.h"

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

class IMetricKey {
public:

  const std::string& getKeyString() const {
    return key_str_;
  }

protected:

  IMetricKey(const std::string& key_str) : key_str_(key_str) {}
  const std::string key_str_;

};

template <typename... D>
class MetricKey : public IMetricKey {
public:

  MetricKey(
      const MetricDescription& description,
      D... dimensions) :
      IMetricKey(buildKeyString(description, dimensions...)) {}

protected:

  std::string buildKeyString(
      const MetricDescription& description,
      D... dimensions) {
    std::stringstream ss;
    FNV<uint64_t> fnv;

    ss << std::hex << fnv.hash(description.getName());

    buildKeyString(&ss, dimensions...);
    return ss.str();
  }

  template<typename... T>
  void buildKeyString(std::stringstream* ss, IDimension head, T... tail) {
    FNV<uint32_t> fnv;

    *ss << '-' << 
        static_cast<char>(head.getTypeId()) <<
        std::hex << fnv.hash(head.getName());

    buildKeyString(ss, tail...);
  }

  void buildKeyString(std::stringstream* ss) {}

};


class IMetric {
public:

  explicit IMetric(
      std::unique_ptr<IStorageCursor>&& cursor,
      const IMetricKey& metric_key,
      const MetricDescription& description,
      const std::vector<IDimension>& dimensions) :
      cursor_(std::move(cursor)),
      metric_key_(metric_key),
      description_(description),
      dimensions_(dimensions) {}

  virtual ~IMetric() {};

  const std::vector<IDimension>& getDimensions() const {
    return dimensions_;
  }

  const IMetricKey& getMetricKey() const {
    return metric_key_;
  }

protected:

  void recordSample(const std::vector<uint8_t>& row) const;
  const std::unique_ptr<IStorageCursor> cursor_;
  const IMetricKey metric_key_;
  const MetricDescription description_;
  const std::vector<IDimension> dimensions_;
};

/**
 * Multidimensional metric (maybe call localmetric?)
 */
template <typename... D>
class Metric : public IMetric {
public:

  explicit Metric(
      std::unique_ptr<IStorageCursor>&& cursor,
      const MetricDescription& description,
      const D... dimensions) :
      IMetric(
          std::move(cursor),
          MetricKey<D...>(description, dimensions...),
          description,
          unpackDimensions(dimensions...)) {}

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
