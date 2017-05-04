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
#include <metrictools/util/return_code.h>

namespace fnordmetric {
class ConfigList;
class IngestionTask;
class Backend;
struct IngestionTaskConfig;

enum class IngestionSampleFormat {
  STATSD, JSON
};

std::string getIngestionSampleFormatName(IngestionSampleFormat t);
bool parseIngestionSampleFormat(const std::string& s, IngestionSampleFormat* t);

ReturnCode mkIngestionTask(
    Backend* storage_backend,
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

  virtual void start() = 0;
  virtual void shutdown() = 0;

};

class PeriodicIngestionTask : public IngestionTask {
public:

  PeriodicIngestionTask(uint64_t interval);

  virtual ReturnCode invoke() = 0;

  void start() override;
  void shutdown() override;

protected:
  uint64_t interval_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool running_;
};

class IngestionService {
public:

  IngestionService(Backend* storage_backend);

  ReturnCode applyConfig(const ConfigList* config);

  void addTask(std::unique_ptr<IngestionTask> task);

  void shutdown();

protected:
  Backend* storage_backend_;
  std::mutex mutex_;
  std::vector<std::pair<std::thread, std::unique_ptr<IngestionTask>>> tasks_;
};

} // namespace fnordmetric

