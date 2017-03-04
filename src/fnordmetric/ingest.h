/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <list>
#include <thread>
#include <regex>
#include <fnordmetric/util/return_code.h>

namespace fnordmetric {
class AggregationService;
class ConfigList;
class IngestionTask;
struct IngestionTaskConfig;

enum class IngestionSampleFormat {
  STATSD, JSON
};

std::string getIngestionSampleFormatName(IngestionSampleFormat t);
bool parseIngestionSampleFormat(const std::string& s, IngestionSampleFormat* t);

ReturnCode mkIngestionTask(
    AggregationService* aggregation_service,
    const IngestionTaskConfig* config,
    std::unique_ptr<IngestionTask>* task);

struct IngestionTaskConfig {
  IngestionTaskConfig();
  virtual ~IngestionTaskConfig() = default;
  bool metric_id_rewrite_enabled;
  std::regex metric_id_rewrite_regex;
  std::string metric_id_rewrite_replace;
};

class IngestionTask {
public:

  virtual ~IngestionTask() = default;

  virtual ReturnCode start() = 0;
  virtual void shutdown() = 0;

};

class PeriodicIngestionTask : public IngestionTask {
public:

  virtual uint64_t getNextInvocationTime() const = 0;
  virtual ReturnCode invoke() = 0;

  ReturnCode start() override;
  void shutdown() override;

};

class IngestionService {
public:

  IngestionService(AggregationService* aggregation_service);

  ReturnCode applyConfig(const ConfigList* config);

  void addTask(std::unique_ptr<IngestionTask> task);

  void shutdown();

protected:
  AggregationService* aggregation_service_;
  std::mutex mutex_;
  std::vector<std::pair<std::thread, std::unique_ptr<IngestionTask>>> tasks_;
};

} // namespace fnordmetric

