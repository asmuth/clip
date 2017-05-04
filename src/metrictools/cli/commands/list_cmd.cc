/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <metrictools/cli/commands/list_cmd.h>
#include <metrictools/util/flagparser.h>
#include <metrictools/storage/ops/fetch_op.h>

namespace fnordmetric {

static ReturnCode listMetric(
    CLIContext* ctx,
    std::shared_ptr<const MetricConfig> metric) {
  FetchStorageOp op(ctx->config->getGlobalConfig());
  op.addRequest(FetchStorageOp::FetchRequest {
    .metric = metric,
    .fetch_last = true,
    .fetch_history = false,
    .history_time_begin = 0,
    .history_time_limit = 0,
  });

  auto rc = ctx->storage_backend->performOperation(&op);
  if (!rc.isSuccess()) {
    return rc;
  }

  std::cout << metric->metric_id << std::endl;
  for (const auto& res : op.getResponses()) {
    std::cout << "    " << res.label << " -> " << res.last_value << std::endl;
  }

  return ReturnCode::success();
}

ReturnCode ListCommand::execute(
    CLIContext* ctx,
    const std::vector<std::string>& argv) {
  FlagParser flags;

  auto flags_rc = flags.parseArgv(argv);
  if (!flags_rc.isSuccess()) {
    return flags_rc;
  }

  switch (flags.getArgv().size()) {

    case 1: {
      auto metric_id = flags.getArgv()[0];
      auto metric = ctx->config->getMetricConfig(metric_id);
      if (!metric) {
        return ReturnCode::errorf(
            "EARG",
            "metric not found: '$0'",
            metric_id);
      }

      return listMetric(ctx, metric);
    }

    case 0: {
      for (const auto& m : ctx->config->getMetricConfigs()) {
        auto rc = listMetric(ctx, m.second);
        if (!rc.isSuccess()) {
          return rc;
        }
      }

      return ReturnCode::success();
    }

    default:
      return ReturnCode::error("EARG", "list requires one or zero arguments");

  }
}

const std::string& ListCommand::getName() const {
  static const std::string kName = "list";
  return kName;
}

const std::string& ListCommand::getDescription() const {
  static const std::string kDescription =
      "List metric labels and their latest values";

  return kDescription;
}

void ListCommand::printHelp() const {
  std::cerr <<
      "Usage: $ metricctl list [<options>] [<metric>]\n"
      "  -F, --filter <expr>       The tree path of the metric.\n"
      "\n"
      "Examples:\n"
      "  $ metrictl list\n"
      "  $ metrictl list load_average -F 'hostname=localhost'\n";
}

} // namespace fnordmetric

