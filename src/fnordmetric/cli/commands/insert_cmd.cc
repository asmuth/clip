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
#include <fnordmetric/cli/commands/insert_cmd.h>
#include <fnordmetric/storage/ops/insert_op.h>
#include <fnordmetric/util/flagparser.h>

namespace fnordmetric {

ReturnCode InsertCommand::execute(
    CLIContext* ctx,
    const std::vector<std::string>& argv) {
  FlagParser flags;

  flags.defineFlag(
      "metric",
      FlagParser::T_STRING,
      true,
      "m",
      NULL);

  flags.defineFlag(
      "tree",
      FlagParser::T_STRING,
      false,
      "t",
      NULL);

  flags.defineFlag(
      "value",
      FlagParser::T_STRING,
      true,
      "v",
      NULL);

  auto flags_rc = flags.parseArgv(argv);
  if (!flags_rc.isSuccess()) {
    return flags_rc;
  }

  auto metric = ctx->config->getMetricConfig(flags.getString("metric"));
  if (!metric) {
    return ReturnCode::errorf(
        "EARG",
        "metric not found: '$0'",
        flags.getString("metric"));
  }

  std::map<std::string, std::string> tree;
  for (const auto& opt : flags.getStrings("tree")) {
    auto opt_key_end = opt.find("=");
    if (opt_key_end == String::npos) {
      return ReturnCode::errorf("EARG", "invalid value for --tree: $0", opt);
    }

    tree.emplace(opt.substr(0, opt_key_end), opt.substr(opt_key_end + 1));
  }

  InsertStorageOp op(ctx->config->getGlobalConfig());
  {
    auto rc = op.addMeasurement(metric, tree, flags.getString("value"));
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
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
      "Usage: $ metricctl insert [OPTIONS]\n"
      "  -m, --metric <name>      The name of the metric into which to insert.\n"
      "  -t, --tree <k>=<v>       The tree path of the metric.\n"
      "  -v, --value <val>        The value to insert.\n"
      "\n"
      "Examples:\n"
      "  $ metricctl insert -m users_online -v 523562\n"
      "  $ metricctl insert -m load_average -t hostname=localhost -v 0.52\n";
}

} // namespace fnordmetric

