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
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <metrictools/cli/commands/collect_cmd.h>
#include <metrictools/util/flagparser.h>

namespace fnordmetric {

int sig_pipe[2] = {-1, -1};

static void sig_handler(int) {
  char shutdown = 'S';
  int rc = write(sig_pipe[1], &shutdown, sizeof(shutdown));
  if (rc != 1) {
    exit(1);
  }
}

ReturnCode CollectCommand::execute(
    CLIContext* ctx,
    const std::vector<std::string>& argv) {
  FlagParser flags;

  flags.defineFlag(
      "once",
      FlagParser::T_SWITCH,
      false,
      "1",
      NULL);

  auto flags_rc = flags.parseArgv(argv);
  if (!flags_rc.isSuccess()) {
    return flags_rc;
  }

  std::unique_ptr<IngestionService> ingestion_service(
      new IngestionService(ctx->storage_backend));

  auto rc = ingestion_service->applyConfig(ctx->config);
  if (!rc.isSuccess()) {
    return rc;
  }

  if (flags.isSet("once")) {
    return ingestion_service->runOnce();
  }

  if (::pipe(sig_pipe) != 0) {
    return ReturnCode::error("EIO", "pipe() failed");
  }

  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);
  signal(SIGHUP, sig_handler);
  signal(SIGPIPE, SIG_IGN);

  ingestion_service->start();

  for (;;) {
    char sig;
    auto read_rc = ::read(sig_pipe[0], &sig, 1);
    if (read_rc != 1) {
      break;
    }

    if (sig == 'S') {
      break;
    }
  }

  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);
  signal(SIGHUP, SIG_DFL);

  ingestion_service->shutdown();
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

