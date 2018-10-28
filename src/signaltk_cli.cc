/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2016 Paul Asmuth, Laura Schlimmer, FnordCorp B.V.
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#include <signaltk/util/flagparser.h>
#include <signaltk/util/fileutil.h>
#include <signaltk/util/return_code.h>
#include <signaltk/util/stringutil.h>

using namespace signaltk;

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

  /* parse flags */
  {
    auto rc = flags.parseArgv(argc, argv);
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return 1;
    }
  }

  auto cmd_argv = flags.getArgv();

  /* print help */
  if (flags.isSet("version")) {
    std::cerr <<
        StringUtil::format(
            "signaltk $0\n"
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
        "Usage: $ signaltk [OPTIONS]\n"
        "   -c, --config <file>       Load config file\n"
        "   -?, --help <topic>        Display a command's help text and exit\n"
        "   -v, --version             Display the version of this binary and exit\n"
        "\n"
        "Commands:\n";

    //for (const auto& c : commands) {
      //stdout_os->printf("   %-26.26s", c->getName().c_str());
      //stdout_os->printf("%-80.80s\n", c->getDescription().c_str());
    //}

    std::cerr << "\nSee 'signaltk help <command>' for information about a specific subcommand.\n";
    return 0;
  }

  /*
  if (help_requested && !help_topic.empty()) {
    const auto& cmd = std::find_if(
        commands.begin(),
        commands.end(),
        [&help_topic] (const auto& c) { return c->getName() == help_topic; });

    if (cmd == commands.end()) {
      std::cerr << StringUtil::format("No manual entry for signaltk '$0'\n", help_topic);
      return 1;
    } else {
      (*cmd)->printHelp();
      return 0;
    }
  }
  */

  /*
  const auto& cmd = std::find_if(
      commands.begin(),
      commands.end(),
      [&cmd_argv] (const auto& c) { return c->getName() == cmd_argv.front(); });

  if (cmd == commands.end()) {
    std::cerr << StringUtil::format(
        "ERROR: '$0' is not a signaltk command. See 'signaltk --help'.\n",
        cmd_argv[0]);

    return 1;
  }

  CLIContext ctx;
  ctx.config = &config;
  ctx.storage_backend = backend.get();

  cmd_argv.erase(cmd_argv.begin());
  auto rc = (*cmd)->execute(&ctx, cmd_argv);

  if (!rc.isSuccess()) {
    std::cerr << StringUtil::format("ERROR: $0\n", rc.getMessage());
  }
  */

  auto rc = ReturnCode::success();

  return rc.isSuccess() ? 0 : 1;
}
