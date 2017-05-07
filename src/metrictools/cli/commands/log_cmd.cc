/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <iostream>
#include <metrictools/cli/commands/log_cmd.h>
#include <metrictools/util/flagparser.h>
#include <metrictools/util/time.h>

namespace fnordmetric {

ReturnCode LogCommand::execute(
    CLIContext* ctx,
    const std::vector<std::string>& argv) {
  FlagParser flags;

  auto flags_rc = flags.parseArgv(argv);
  if (!flags_rc.isSuccess()) {
    return flags_rc;
  }

  if (flags.getArgv().size() != 1) {
    return ReturnCode::error("EARG", "log requires exactly 1 argument");
  }

  auto metric_id = flags.getArgv()[0];
  auto metric = ctx->config->getMetricConfig(metric_id);
  if (!metric) {
    return ReturnCode::errorf(
        "EARG",
        "metric not found: '$0'",
        metric_id);
  }

  const UnitConfig* unit = nullptr;
  if (!metric->unit_id.empty()) {
    unit = ctx->config->getUnitConfig(metric->unit_id);
  }

  FetchStorageOp op(ctx->config->getGlobalConfig());
  op.addRequest(FetchStorageOp::FetchRequest {
    .metric = metric,
    .fetch_last = true,
    .fetch_history = true,
    .history_time_begin = WallClock::unixMicros() - 2 * kMicrosPerSecond,
    .history_time_limit = WallClock::unixMicros()
  });

  auto rc = ctx->storage_backend->performOperation(&op);
  if (!rc.isSuccess()) {
    return rc;
  }

  for (const auto& res : op.getResponses()) {
    Timeseries<std::string> res_ts;
    auto rc = convertTimeseries(res.history.get(), &res_ts);
    if (!rc.isSuccess()) {
      return rc;
    }

    std::cout << metric->metric_id << " :: " << res.label << std::endl;
    for (size_t i = 0; i < res_ts.timestamps.size(); ++i) {
      std::cout
          << "  " << UnixTime(res_ts.timestamps[i]).toString()
          << " -> " << formatValue(res_ts.values[i], unit, metric->rate)
          << std::endl;
    }

    std::cout << std::endl;
  }

  return ReturnCode::success();
}

const std::string& LogCommand::getName() const {
  static const std::string kName = "log";
  return kName;
}

const std::string& LogCommand::getDescription() const {
  static const std::string kDescription =
      "Log the historical values for one or more metric labels";

  return kDescription;
}

void LogCommand::printHelp() const {
  std::cerr <<
      "Usage: $ metricctl log [<options>] <metric>\n"
      "  -F, --filter <expr>       The tree path of the metric.\n"
      "\n"
      "Examples:\n"
      "  $ metrictl log load_average\n"
      "  $ metrictl log load_average -F 'hostname=localhost'\n";
}

} // namespace fnordmetric

