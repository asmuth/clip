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
#include "fnordmetric/table_map.h"
#include "fnordmetric/config_list.h"
#include "fnordmetric/util/return_code.h"

namespace fnordmetric {

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

} // namespace fnordmetric

