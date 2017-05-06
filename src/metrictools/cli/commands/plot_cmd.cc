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
#include <metrictools/cli/commands/plot_cmd.h>
#include <metrictools/util/flagparser.h>
#include <metrictools/storage/ops/fetch_op.h>

namespace fnordmetric {

ReturnCode PlotCommand::execute(
    CLIContext* ctx,
    const std::vector<std::string>& argv) {
  FlagParser flags;

  auto flags_rc = flags.parseArgv(argv);
  if (!flags_rc.isSuccess()) {
    return flags_rc;
  }

  return ReturnCode::success();
}

const std::string& PlotCommand::getName() const {
  static const std::string kName = "plot";
  return kName;
}

const std::string& PlotCommand::getDescription() const {
  static const std::string kDescription =
      "Plot one or more metrics";

  return kDescription;
}

void PlotCommand::printHelp() const {
  std::cerr <<
      "Usage: $ metricctl plot [<options>] [<metric>]\n"
      "  -m, --metric <name>       Add a metric to plot\n"
      "  -x, --filter <expr>       Filter series in the last metric.\n"
      "\n"
      "Examples:\n"
      "  $ metrictl plot -m linux.load_avg_1m\n"
      "  $ metrictl plot -m linux.load_avg_1m -x hostname=localhost\n";
}

} // namespace fnordmetric

