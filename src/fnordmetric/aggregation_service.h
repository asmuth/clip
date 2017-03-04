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
#include "fnordmetric/table.h"
#include "fnordmetric/util/sha1.h"
#include "fnordmetric/table_map.h"
#include "fnordmetric/config_list.h"
#include "fnordmetric/util/return_code.h"

namespace fnordmetric {
class AggregationMap;

class AggregationService {
public:

  static ReturnCode startService(std::unique_ptr<AggregationService>* service);

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

protected:

  AggregationService();

  VersionedTableMap table_map_;
  std::mutex table_map_mutex_;
};

class AggregationMap {
public:

  struct Slot {
    SHA1Hash slot_id;
    uint64_t time;
    uint64_t interval;
    std::shared_ptr<TableConfig> table;
    std::vector<std::pair<std::string, std::string>> labels;
  };

  Slot* getSlot(
      uint64_t timestamp,
      uint64_t interval,
      uint64_t expire_at,
      std::shared_ptr<TableConfig> table,
      const std::vector<std::pair<std::string, std::string>>& labels);

  void getExpiredSlots(
      uint64_t expired_on,
      std::vector<std::unique_ptr<Slot>>* slots);

protected:
  std::multimap<SHA1Hash, Slot*> slots_;
  std::deque<std::pair<uint64_t, std::unique_ptr<Slot>>> expiration_list_;
};

} // namespace fnordmetric

