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
#include <metrictools/metric.h>
#include <metrictools/measure.h>
#include <metrictools/task.h>

namespace fnordmetric {
class ConfigList;
class Backend;

struct ListenerConfig {
  virtual ~ListenerConfig() = default;
};

ReturnCode mkListenerTask(
    Backend* storage_backend,
    const ConfigList* config_list,
    const ListenerConfig* config,
    std::unique_ptr<Task>* task);

ReturnCode startListeners(
    Backend* storage_backend,
    const ConfigList* config_list,
    TaskRunner* task_runner);

} // namespace fnordmetric

