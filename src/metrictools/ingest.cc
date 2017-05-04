/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <unistd.h>
#include <metrictools/ingest.h>
#include <metrictools/util/time.h>
#include <metrictools/util/logging.h>
#include <metrictools/listen_udp.h>
#include <metrictools/listen_http.h>
#include <metrictools/fetch_http.h>
#include <metrictools/config_list.h>
#include <metrictools/statsd.h>
#include <metrictools/storage/ops/insert_op.h>
#include <metrictools/storage/backend.h>

namespace fnordmetric {

std::string getIngestionSampleFormatName(IngestionSampleFormat t) {
  switch (t) {
    case IngestionSampleFormat::STATSD: return "statsd";
    case IngestionSampleFormat::JSON: return "json";
  }

  return "???";
}

bool parseIngestionSampleFormat(
    const std::string& s,
    IngestionSampleFormat* t) {
  if (s == "statsd") { *t = IngestionSampleFormat::STATSD; return true; }
  if (s == "json") { *t = IngestionSampleFormat::JSON; return true; }
  return false;
}

ReturnCode mkIngestionTask(
    Backend* storage_backend,
    const ConfigList* config_list,
    const IngestionTaskConfig* config,
    std::unique_ptr<IngestionTask>* task) {
  if (dynamic_cast<const UDPIngestionTaskConfig*>(config)) {
    return UDPListener::start(storage_backend, config, task);
  }

  if (dynamic_cast<const HTTPPushIngestionTaskConfig*>(config)) {
    return HTTPPushIngestionTask::start(storage_backend, config, task);
  }

  if (dynamic_cast<const HTTPPullIngestionTaskConfig*>(config)) {
    return HTTPPullIngestionTask::start(storage_backend, config_list, config, task);
  }

  return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
}

ReturnCode parseSamples(
    IngestionSampleFormat format,
    const std::string& input,
    std::vector<IngestionSample>* samples) {
  switch (format) {
    case IngestionSampleFormat::STATSD:
      return parseStatsdSamples(input.data(), input.size(), samples);
    default:
      return ReturnCode::error("ERUNTIME", "invalid format");
  }
}

ReturnCode storeSamples(
    const ConfigList* config,
    Backend* storage_backend,
    const std::vector<IngestionSample>& samples) {
  InsertStorageOp op(config->getGlobalConfig());
  for (const auto& s : samples) {
    auto metric = config->getMetricConfig(s.metric_id);
    if (!metric) {
      logWarning(
          "batch insert failed: metric not found: '$0'",
          s.metric_id);
      continue;
    }

    auto rc = op.addMeasurement(metric, s.instance, s.value);
    if (!rc.isSuccess()) {
      logWarning(
          "batch insert failed: $0; metric_id=$1 value=$2",
          rc.getMessage(),
          s.metric_id,
          s.value);
    }
  }

  return storage_backend->performOperation(&op);
}

IngestionTaskConfig::IngestionTaskConfig() :
    metric_id_rewrite_enabled(false) {}

PeriodicIngestionTask::PeriodicIngestionTask(
    uint64_t interval) :
    interval_(interval),
    running_(false) {}

void PeriodicIngestionTask::start() {
  uint64_t last_run = 0;

  std::unique_lock<std::mutex> lk(mutex_);
  running_ = true;
  while (running_) {
    auto now = MonotonicClock::now();
    if (now - last_run < interval_) {
      cv_.wait_for(lk, std::chrono::microseconds(interval_ - (now - last_run)));
    }

    if (!running_) {
      break;
    }

    last_run = now;
    lk.unlock();

    auto rc = ReturnCode::success();
    try {
      rc = invoke();
    } catch (const std::exception& e) {
      rc = ReturnCode::error("ERUNTIME", e.what());
    }

    if (!rc.isSuccess()) {
      logWarning("Ingestion task failed: $0", rc.getMessage());
    }

    lk.lock();
  }
}

void PeriodicIngestionTask::shutdown() {
  std::unique_lock<std::mutex> lk(mutex_);
  running_ = false;
  cv_.notify_all();
}

IngestionService::IngestionService(
    Backend* storage_backend) :
    storage_backend_(storage_backend) {}

ReturnCode IngestionService::applyConfig(const ConfigList* config) {
  for (const auto& ic : config->getIngestionTaskConfigs()) {
    std::unique_ptr<IngestionTask> task;
    auto rc = mkIngestionTask(storage_backend_, config, ic.get(), &task);
    if (!rc.isSuccess()) {
      return rc;
    }

    addTask(std::move(task));
  }

  return ReturnCode::success();
}

void IngestionService::addTask(std::unique_ptr<IngestionTask> task) {
  std::unique_lock<std::mutex> lk(mutex_);
  tasks_.emplace_back(std::move(task));
}

void IngestionService::start() {
  for (auto& t : tasks_) {
    threads_.emplace_back(
        std::thread(std::bind(&IngestionTask::start, t.get())));
  }
}

void IngestionService::shutdown() {
  std::unique_lock<std::mutex> lk(mutex_);
  for (auto& t : tasks_) {
    t->shutdown();
  }

  for (auto& t : threads_) {
    t.join();
  }
}

} // namespace fnordmetric

