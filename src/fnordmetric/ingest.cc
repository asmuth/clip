/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/ingest.h>
#include <fnordmetric/util/time.h>
#include <fnordmetric/util/logging.h>
#include <fnordmetric/listen_udp.h>
#include <fnordmetric/listen_http.h>

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
    AggregationService* service,
    const IngestionTaskConfig* config,
    std::unique_ptr<IngestionTask>* task) {
  if (dynamic_cast<const UDPIngestionTaskConfig*>(config)) {
    return UDPListener::start(service, config, task);
  }

  return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
}

IngestionTaskConfig::IngestionTaskConfig() :
    metric_id_rewrite_enabled(false) {}

IngestionService::IngestionService(
    AggregationService* aggregation_service) :
    aggregation_service_(aggregation_service) {}

ReturnCode IngestionService::applyConfig(const ConfigList* config) {
  for (const auto& ic : config->getIngestionTaskConfigs()) {
    std::unique_ptr<IngestionTask> task;
    auto rc = mkIngestionTask(aggregation_service_, ic.get(), &task);
    if (!rc.isSuccess()) {
      return rc;
    }

    addTask(std::move(task));
  }

  return ReturnCode::success();
}

void IngestionService::addTask(std::unique_ptr<IngestionTask> task) {
  std::unique_lock<std::mutex> lk(mutex_);
  auto task_ptr = task.get();
  tasks_.emplace_back(
      std::thread(std::bind(&IngestionTask::start, task_ptr)),
      std::move(task));
}

void IngestionService::shutdown() {
  std::unique_lock<std::mutex> lk(mutex_);
  for (auto& t : tasks_) {
    t.second->shutdown();
    t.first.join();
  }

  tasks_.clear();
}

} // namespace fnordmetric

