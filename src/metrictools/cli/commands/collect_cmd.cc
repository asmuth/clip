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
#include <metrictools/cli/commands/collect_cmd.h>
#include <metrictools/util/flagparser.h>

namespace fnordmetric {

ReturnCode CollectCommand::execute(
    CLIContext* ctx,
    const std::vector<std::string>& argv) {
  FlagParser flags;

  auto flags_rc = flags.parseArgv(argv);
  if (!flags_rc.isSuccess()) {
    return flags_rc;
  }

  std::unique_ptr<IngestionService> ingestion_service(
      new IngestionService(ctx->storage_backend));

  {
    auto rc = ingestion_service->applyConfig(ctx->config);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}

const std::string& CollectCommand::getName() const {
  static const std::string kName = "collect";
  return kName;
}

const std::string& CollectCommand::getDescription() const {
  static const std::string kDescription =
      "Collect new measurements from sensors";

  return kDescription;
}

void CollectCommand::printHelp() const {
  std::cerr <<
      "Usage: $ metricctl collect [<options>]\n"
      "  -1, --once                Execute the collection routine once and exit.\n"
      "\n"
      "Examples:\n"
      "  $ metrictl collect --once\n";
}

} // namespace fnordmetric

