/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/config_list.h>
#include <metrictools/listen.h>
#include <metrictools/listen_http.h>
#include <metrictools/listen_udp.h>

namespace fnordmetric {

ReturnCode mkTask(
    Backend* storage_backend,
    const ConfigList* config_list,
    const ListenerConfig* config,
    std::unique_ptr<Task>* task) {
  if (dynamic_cast<const ListenHTTPTaskConfig*>(config)) {
    return startHTTPListener(storage_backend, config_list, config, task);
  }

  if (dynamic_cast<const ListenUDPTaskConfig*>(config)) {
    return UDPListener::start(storage_backend, config_list, config, task);
  }

  return ReturnCode::error("ERUNTIME", "invalid listener config");
}

ReturnCode startListeners(
    Backend* storage_backend,
    const ConfigList* config_list,
    TaskRunner* task_runner) {
  for (const auto& ic : config_list->getListenerConfigs()) {
    std::unique_ptr<Task> task;
    auto rc = mkTask(storage_backend, config_list, ic.get(), &task);
    if (!rc.isSuccess()) {
      return rc;
    }

    task_runner->addTask(std::move(task));
  }

  return ReturnCode::success();
}


} // namespace fnordmetric

