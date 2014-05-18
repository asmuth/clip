/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_AGENT_H
#define _FNORDMETRIC_AGENT_H

#include <memory>
#include "metric.h"
#include "storagebackend.h"

namespace fnordmetric {

/**
 * Agent
 */
class Agent {
public:

  Agent(
    const std::string& name,
    std::unique_ptr<IStorageBackend>&& storage_backend) :
    name_(name),
    storage_backend_(std::move(storage_backend)) {}

  Agent(const Agent& copy) = delete;

  template <typename... D>
  std::shared_ptr<const Metric<D...>> newMetric(
      const MetricDescription& description,
      const D... dimensions) {
    auto metric = std::make_shared<const Metric<D...>>(
        this,
        description,
        dimensions...);

    // FIXPAUL make threadsafe!
    metrics_.push_back(metric);

    return metric;
  }

protected:

  std::string name_;
  std::vector<std::shared_ptr<const IMetric>> metrics_;
  std::unique_ptr<IStorageBackend> storage_backend_;

};

}

#endif
