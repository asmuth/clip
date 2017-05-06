/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <regex>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/file.h>
#include <metrictools/util/flagparser.h>
#include <metrictools/util/logging.h>
#include <metrictools/util/fileutil.h>
#include <metrictools/util/daemonize.h>
#include <metrictools/util/return_code.h>
#include <metrictools/config_list.h>
#include <metrictools/config_parser.h>
#include <metrictools/collect.h>
#include <metrictools/cli/command.h>
#include <metrictools/cli/commands/collect_cmd.h>
#include <metrictools/cli/commands/insert_cmd.h>
#include <metrictools/cli/commands/log_cmd.h>
#include <metrictools/cli/commands/list_cmd.h>

using namespace fnordmetric;

int main(int argc, const char** argv) {
  FlagParser flags;

  flags.defineFlag(
      "config",
      FlagParser::T_STRING,
      false,
      "c",
      NULL);

  flags.defineFlag(
      "help",
      FlagParser::T_STRING,
      false,
      "?",
      "");

  flags.defineFlag(
      "version",
      FlagParser::T_SWITCH,
      false,
      "V",
      NULL);

  flags.defineFlag(
      "verbose",
      FlagParser::T_SWITCH,
      false,
      "v",
      NULL);

  /* parse flags */
  {
    auto rc = flags.parseArgv(argc, argv);
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return 1;
    }
  }

  bool verbose = flags.isSet("verbose");
  auto cmd_argv = flags.getArgv();
  Logger::logToStderrWithoutDecoration();
  if (verbose) {
    Logger::get()->setMinimumLogLevel(strToLogLevel("DEBUG"));
  } else {
    Logger::get()->setMinimumLogLevel(strToLogLevel("WARNING"));
  }

  /* init commands */
  std::vector<std::unique_ptr<Command>> commands;
  commands.emplace_back(new ListCommand());
  commands.emplace_back(new LogCommand());
  commands.emplace_back(new CollectCommand());
  commands.emplace_back(new InsertCommand());

  /* print help */
  if (flags.isSet("version")) {
    std::cerr <<
        StringUtil::format(
            "metricctl $0\n"
            "Part of the FnordMetric project (http://fnordmetric.io)\n"
            "Copyright (c) 2016, Paul Asmuth et al. All rights reserved.\n\n",
            FNORDMETRIC_VERSION);

    return 0;
  }

  bool help_requested = false;
  std::string help_topic;

  if (flags.isSetExplicit("help")) {
    help_requested = true;
    help_topic = flags.getString("help");
  }
 
  if (cmd_argv.size() > 0 && cmd_argv[0] == "help") {
    help_requested = true;
    help_topic = cmd_argv.size() > 1 ? cmd_argv[1] : "";
  }

  if (help_requested && help_topic.empty()) {
    std::cerr <<
        "Usage: $ metricctl [OPTIONS]\n"
        "   -c, --config <file>       Load config file\n"
        "   -v, --verbose             Run in verbose mode\n"
        "   -?, --help <topic>        Display a command's help text and exit\n"
        "   -V, --version             Display the version of this binary and exit\n"
        "\n"
        "Commands:\n";

    for (const auto& c : commands) {
      //stdout_os->printf("   %-26.26s", c->getName().c_str());
      //stdout_os->printf("%-80.80s\n", c->getDescription().c_str());
    }

    std::cerr << "\nSee 'metricctl help <command>' for information about a specific subcommand.\n";
    return 0;
  }

  if (help_requested && !help_topic.empty()) {
    const auto& cmd = std::find_if(
        commands.begin(),
        commands.end(),
        [&help_topic] (const auto& c) { return c->getName() == help_topic; });

    if (cmd == commands.end()) {
      std::cerr << StringUtil::format("No manual entry for metrictl '$0'\n", help_topic);
      return 1;
    } else {
      (*cmd)->printHelp();
      return 0;
    }
  }

  /* search for config */
  std::string config_path;
  if (flags.isSet("config")) {
    config_path = flags.getString("config");
  } else {
    std::vector<std::string> candidates = {
      FileUtil::joinPaths(getenv("HOME"), ".metrictl.conf"),
      "/etc/metrictools/metrictl.conf",
      "/etc/metrictl.conf"
    };

    for (const auto& c : candidates) {
      if (FileUtil::exists(c)) {
        config_path = c;
        break;
      }
    }

    if (config_path.empty()) {
      std::cerr << "ERROR: no config file found (--config), tried:" << std::endl;
      for (const auto& c : candidates) {
        std::cerr << "ERROR:   - " << c << std::endl;
      }
      return 1;
    }
  }

  /* load config */
  ConfigList config;
  {
    std::unique_ptr<ConfigParser> config_parser;
    auto rc = ConfigParser::openFile(&config_parser, config_path);
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return 1;
    }

    rc = config_parser->parse(&config);
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return 1;
    }
  }

  /* open backend */
  if (config.getBackendURL().empty()) {
   std::cerr << "ERROR: missing backend url";
   return 1;
  }

  std::unique_ptr<Backend> backend;
  {
    auto rc = Backend::openBackend(config.getBackendURL(), &backend);
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return 1;
    }
  }

  if (cmd_argv.empty()) {
    std::cerr << "ERROR: command is not specified. See 'metrictl --help'.\n";
    return 1;
  }

  /* execute command */
  const auto& cmd = std::find_if(
      commands.begin(),
      commands.end(),
      [&cmd_argv] (const auto& c) { return c->getName() == cmd_argv.front(); });

  if (cmd == commands.end()) {
    std::cerr << StringUtil::format(
        "ERROR: '$0' is not a metrictl command. See 'metrictl --help'.\n",
        cmd_argv[0]);

    return 1;
  }

  CLIContext ctx;
  ctx.config = &config;
  ctx.storage_backend = backend.get();
  ctx.verbose = verbose;

  cmd_argv.erase(cmd_argv.begin());
  auto rc = (*cmd)->execute(&ctx, cmd_argv);

  if (!rc.isSuccess()) {
    std::cerr << StringUtil::format("ERROR: $0\n", rc.getMessage());
  }

  return rc.isSuccess() ? 0 : 1;
}

