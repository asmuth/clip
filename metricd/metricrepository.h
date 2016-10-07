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
#include <metricd/metric.h>
#include <mutex>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fnordmetric {

class MetricRepository {
public:

  virtual ~MetricRepository() {}

  Metric* findMetric(const std::string& key) const;

  std::vector<Metric*> listMetrics() const;

protected:
  std::unordered_map<std::string, std::unique_ptr<Metric>> metrics_;
  mutable std::mutex metrics_mutex_;
};

} // namespace fnordmetric

