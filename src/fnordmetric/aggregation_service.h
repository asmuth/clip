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
#include <atomic>
#include <memory>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "fnordmetric/table.h"
#include "fnordmetric/util/sha1.h"
#include "fnordmetric/table_map.h"
#include "fnordmetric/config_list.h"
#include "fnordmetric/util/return_code.h"
#include "fnordmetric/backends/backend.h"

namespace fnordmetric {
struct AggregationSlot;
class AggregationMap;

class AggregationService {
public:

  static ReturnCode startService(
      Backend* backend,
      std::unique_ptr<AggregationService>* service);

  ~AggregationService();

  ReturnCode applyConfig(const ConfigList* config);

  ReturnCode insertSample(const Sample& sample);

  struct BatchInsertOptions {
    BatchInsertOptions();
    bool metric_id_rewrite_enabled;
    std::regex metric_id_rewrite_regex;
    std::string metric_id_rewrite_replace;
  };

  ReturnCode insertSamplesBatch(
      const char* data,
      size_t len,
      const BatchInsertOptions* opts = nullptr);

  void shutdown();

protected:

  AggregationService(Backend* backend);

  void processExpirations();
  void performInserts(std::vector<std::unique_ptr<AggregationSlot>> slots);

  Backend* backend_;
  VersionedTableMap table_map_;
  std::mutex table_map_mutex_;
  std::unique_ptr<AggregationMap> aggregation_map_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool expiration_thread_shutdown_;
  std::thread expiration_thread_;
};

struct AggregationSlot {
  SHA1Hash slot_id;
  uint64_t timestamp;
  std::shared_ptr<TableConfig> table;
  std::vector<std::string> labels;
  std::vector<std::unique_ptr<AggregationFunction>> measures;
};

class AggregationMap {
public:

  ReturnCode getSlot(
      uint64_t timestamp,
      uint64_t expire_at,
      std::shared_ptr<TableConfig> table,
      const std::vector<std::string>& labels,
      AggregationSlot** slot);

  void getExpiredSlots(
      uint64_t expired_on,
      std::vector<std::unique_ptr<AggregationSlot>>* slots);

  uint64_t getNextExpiration() const;

protected:
  std::multimap<SHA1Hash, AggregationSlot*> slots_;
  std::deque<std::pair<uint64_t, std::unique_ptr<AggregationSlot>>> expiration_list_;
};

uint64_t alignTime(uint64_t timestamp, uint64_t window, uint64_t align = 0);

} // namespace fnordmetric

