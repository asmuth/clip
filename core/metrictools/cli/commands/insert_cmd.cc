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
#include <metrictools/cli/commands/insert_cmd.h>
#include <metrictools/storage/ops/insert_op.h>
#include <metrictools/util/flagparser.h>

namespace fnordmetric {

ReturnCode InsertCommand::execute(
    CLIContext* ctx,
    const std::vector<std::string>& argv) {
  FlagParser flags;

  auto flags_rc = flags.parseArgv(argv);
  if (!flags_rc.isSuccess()) {
    return flags_rc;
  }

  const auto& cmd_argv = flags.getArgv();
  if (cmd_argv.size() < 2) {
    return ReturnCode::error("EARG", "insert requires two or more arguments");
  }

  auto metric_id = cmd_argv[0];
  auto value = cmd_argv[cmd_argv.size() - 1];

  auto metric = ctx->config->getMetricConfig(metric_id);
  if (!metric) {
    return ReturnCode::errorf(
        "EARG",
        "metric not found: '$0'",
        metric_id);
  }

  LabelSet labels;
  for (size_t i = 1; i < cmd_argv.size() - 1; ++i) {
    const auto& lbl = cmd_argv[i];
    auto lbl_key_end = lbl.find("=");
    if (lbl_key_end == String::npos) {
      return ReturnCode::errorf(
          "EARG",
          "invalid label: $0 -- format is <key>=<value>",
          lbl);
    }

    labels.emplace(lbl.substr(0, lbl_key_end), lbl.substr(lbl_key_end + 1));
  }

  InsertStorageOp op(ctx->config->getGlobalConfig());
  {
    auto rc = op.addMeasurement(metric, labels, value);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return ctx->storage_backend->performOperation(&op);
}

const std::string& InsertCommand::getName() const {
  static const std::string kName = "insert";
  return kName;
}

const std::string& InsertCommand::getDescription() const {
  static const std::string kDescription =
      "Record a new measurement to the datastore";

  return kDescription;
}

void InsertCommand::printHelp() const {
  std::cerr <<
      "Usage: $ metricctl insert [<options>] <metric> [<label>=<label>]... <value>\n"
      "\n"
      "Examples:\n"
      "  $ metrictl insert -m users_online -v 523562\n"
      "  $ metrictl insert -m load_average -t hostname=localhost -v 0.52\n";
}

} // namespace fnordmetric

