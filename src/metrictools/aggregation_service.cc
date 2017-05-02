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
#include <assert.h>
#include <iostream>
#include <sstream>
#include "metrictools/aggregation_service.h"
#include "metrictools/util/fileutil.h"
#include "metrictools/util/logging.h"
#include "metrictools/util/time.h"
#include "metrictools/util/sha1.h"
#include "metrictools/statsd.h"

namespace fnordmetric {

ReturnCode AggregationService::startService(
    Backend* backend,
    std::unique_ptr<AggregationService>* service) {
  service->reset(new AggregationService(backend));
  return ReturnCode::success();
}

AggregationService::AggregationService(
    Backend* backend) :
    backend_(backend),
    aggregation_map_(new AggregationMap()),
    expiration_thread_shutdown_(false),
    expiration_thread_(std::thread(
        std::bind(&AggregationService::processExpirations, this))) {}

AggregationService::~AggregationService() {
  shutdown();

  if (expiration_thread_.joinable()) {
    expiration_thread_.join();
  }
}

ReturnCode AggregationService::applyConfig(const ConfigList* config) {
  if (config->getCreateTables()) {
    for (auto& mc : config->getTableConfigs()) {
      auto rc = backend_->createTable(mc.second);
      if (!rc.isSuccess()) {
        return rc;
      }
    }
  }

  std::unique_lock<std::mutex> lk(table_map_mutex_);
  TableMapBuilder table_map_builder;
  for (auto& mc : config->getTableConfigs()) {
    table_map_builder.addTable(mc.first, mc.second);
  }

  table_map_.updateTableMap(table_map_builder.getTableMap());
  return ReturnCode::success();
}

static bool parseMetricName(
    const std::string& metric_name,
    std::string* table_id,
    std::string* measure_id) {
  size_t table_id_len = metric_name.size();
  while (table_id_len > 0) {
    if (metric_name[table_id_len - 1] == '.') {
      break;
    } else {
      --table_id_len;
    }
  }

  if (table_id_len <= 1 || table_id_len == metric_name.size()) {
    return false;
  } else {
    *table_id = metric_name.substr(0, table_id_len - 1);
    *measure_id = metric_name.substr(table_id_len);
    return true;
  }
}

ReturnCode AggregationService::insertSample(const Sample& smpl) {
  std::string table_id;
  std::string measure_id;
  if (!parseMetricName(smpl.getMetricName(), &table_id, &measure_id)) {
    return ReturnCode::errorf(
        "ERUNTIME",
        "invalid metric name: $0",
        smpl.getMetricName());
  }

  logDebug(
      "Ingesting sample: metric_name=$0 value=$1",
      smpl.getMetricName(),
      smpl.getValue());

  auto table = table_map_.getTableMap()->findTable(table_id);
  if (!table.get()) {
    return ReturnCode::errorf("ERUNTIME", "table not found: $0", table_id);
  }

  size_t measure_idx = size_t(-1);
  for (size_t i = 0; i < table->measures.size(); ++i) {
    if (table->measures[i].column_name == measure_id) {
      measure_idx = i;
      break;
    }
  }

  if (measure_idx == size_t(-1)) {
    return ReturnCode::errorf("ERUNTIME", "column not found: $0", measure_id);
  }

  std::vector<std::string> labels(table->labels.size());
  for (size_t i = 0; i < labels.size(); ++i) {
    for (const auto& l : smpl.getLabels()) {
      if (l.first == table->labels[i].column_name) {
        labels[i] = l.second;
        break;
      }
    }
  }

  auto now = MonotonicClock::now();
  std::unique_lock<std::mutex> lk(mutex_);

  AggregationSlot* slot;
  auto rc = aggregation_map_->getSlot(
      alignTime(smpl.getTime(), table->interval),
      now + table->interval,
      table,
      labels,
      &slot);

  if (rc.isSuccess()) {
    slot->measures[measure_idx]->addSample(table->measures[measure_idx], smpl);
    cv_.notify_all();
    return ReturnCode::success();
  } else {
    return rc;
  }
}

AggregationService::BatchInsertOptions::BatchInsertOptions() :
    metric_id_rewrite_enabled(false) {}

ReturnCode AggregationService::insertSamplesBatch(
    const char* data,
    size_t len,
    const BatchInsertOptions* opts /* = nullptr */) {
  std::string metric_id;
  std::string series_id;
  std::string value;
  LabelSet labels;

  char const* cur = data;
  char const* end = data + len;

  while (cur < end) {
    if (!parseStatsdSample(&cur, end, &metric_id, &series_id, &value)) {
      return ReturnCode::error("EPARSE", "invalid packet");
    }

    if (opts && opts->metric_id_rewrite_enabled) {
      try {
        metric_id = std::regex_replace(
            metric_id,
            opts->metric_id_rewrite_regex,
            opts->metric_id_rewrite_replace,
            std::regex_constants::match_default |
            std::regex_constants::format_no_copy |
            std::regex_constants::format_first_only);

      } catch (const std::exception& e) {
        return ReturnCode::errorf("ERUNTIME", "regex error: $0", e.what());
      }
    }

    auto now = WallClock::unixMicros();
    auto rc = insertSample(Sample(metric_id, value, now, {}));

    if (!rc.isSuccess()) {
      logWarning(
          "batch insert failed: $0; metric_id=$1 value=$2",
          rc.getMessage(),
          metric_id,
          value);
    }
  }

  return ReturnCode::success();
}

void AggregationService::shutdown() {
  std::unique_lock<std::mutex> lk(mutex_);
  expiration_thread_shutdown_ = true;
  cv_.notify_all();
}

void AggregationService::processExpirations() {
  std::unique_lock<std::mutex> lk(mutex_);
  while (!expiration_thread_shutdown_) {
    auto now = MonotonicClock::now();
    std::vector<std::unique_ptr<AggregationSlot>> expired;
    aggregation_map_->getExpiredSlots(now, &expired);

    if (!expired.empty()) {
      performInserts(std::move(expired));
    }

    auto next = aggregation_map_->getNextExpiration();
    if (next == 0) {
      cv_.wait(lk);
    } else {
      assert(next > now);
      cv_.wait_for(lk, std::chrono::microseconds(next - now));
    }
  }
}

void AggregationService::performInserts(
    std::vector<std::unique_ptr<AggregationSlot>> slots) {
  std::vector<Backend::InsertOp> insert_ops;

  for (auto& s : slots) {
    logDebug(
        "Emitting row: table=$0 window=$1",
        s->table->table_id,
        UnixTime(s->timestamp).toString());

    Backend::InsertOp op;
    op.table = s->table;
    op.time = s->timestamp;

    for (size_t i = 0; i < s->labels.size(); ++i) {
      op.columns.emplace_back(s->table->labels[i].column_name, s->labels[i]);
    }

    for (size_t i = 0; i < s->measures.size(); ++i) {
      s->measures[i]->getResult(s->table->measures[i], &op.columns);
    }

    insert_ops.emplace_back(std::move(op));
  }

  auto rc = backend_->insertRows(std::move(insert_ops));
  if (!rc.isSuccess()) {
    logError("BACKEND ERROR: $0", rc.getMessage());
  }
}

ReturnCode AggregationMap::getSlot(
    uint64_t timestamp,
    uint64_t expire_at,
    std::shared_ptr<TableConfig> table,
    const std::vector<std::string>& labels,
    AggregationSlot** slot) {
  std::string slot_idv;
  slot_idv += std::to_string(timestamp) + "~";
  slot_idv += std::to_string((intptr_t) table.get());
  for (const auto& l : labels) {
    slot_idv += "~" + l;
  }

  auto slot_id = SHA1::compute(slot_idv);
  auto slots = slots_.equal_range(slot_id);
  for (auto iter = slots.first; iter != slots.second; ++iter) {
    if (iter->second->timestamp == timestamp &&
        iter->second->table.get() == table.get() &&
        iter->second->labels == labels) {
      *slot = iter->second;
      return ReturnCode::success();
    }
  }

  auto new_slot = std::unique_ptr<AggregationSlot>(new AggregationSlot());
  new_slot->slot_id = slot_id;
  new_slot->timestamp = timestamp;
  new_slot->table = table;
  new_slot->labels = labels;

  for (const auto& m : table->measures) {
    std::unique_ptr<AggregationFunction> aggregation_fun;
    auto rc = mkAggregationFunction(m, &aggregation_fun);
    if (!rc.isSuccess()) {
      return rc;
    }

    new_slot->measures.emplace_back(std::move(aggregation_fun));
  }

  *slot = new_slot.get();
  slots_.emplace(slot_id, new_slot.get());

  auto iter = expiration_list_.begin();
  for (; iter->first > expire_at && iter != expiration_list_.end(); ++iter);
  expiration_list_.insert(iter, std::make_pair(expire_at, std::move(new_slot)));
  return ReturnCode::success();
}

void AggregationMap::getExpiredSlots(
    uint64_t expired_on,
    std::vector<std::unique_ptr<AggregationSlot>>* slots) {
  while (!expiration_list_.empty() && expiration_list_.back().first <= expired_on) {
    auto slot = std::move(expiration_list_.back().second);
    expiration_list_.pop_back();

    auto iter_range = slots_.equal_range(slot->slot_id);
    for (auto iter = iter_range.first; iter != iter_range.second; ++iter) {
      if (iter->second == slot.get()) {
        slots_.erase(iter);
        break;
      }
    }

    slots->emplace_back(std::move(slot));
  }
}

uint64_t AggregationMap::getNextExpiration() const {
  if (expiration_list_.empty()) {
    return 0;
  }

  return expiration_list_.back().first;
}

uint64_t alignTime(uint64_t timestamp, uint64_t window, uint64_t align /* = 0 */) {
  if (window >= std::numeric_limits<uint64_t>::max() / 2 ||
      timestamp >= std::numeric_limits<uint64_t>::max() / 2) {
    return -1;
  }

  int64_t timestamp_base = int64_t(timestamp) - int64_t(align);
  return (timestamp_base / int64_t(window)) * int64_t(window) + int64_t(align);
}

} // namsepace fnordmetric

