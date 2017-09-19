/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://ww
 */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <algorithm>
#include <zdb/util/flagparser.h>
#include <zdb/util/fileutil.h>
#include <zdb/util/return_code.h>
#include <zdb/zdbtool_init.h>

using namespace zdb;

struct Command {
  std::string name;
  std::string description;
  std::string help;
  std::function<ReturnCode (const std::vector<std::string>&)> call;
};

int main(int argc, const char** argv) {
  FlagParser flags;

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

  /* init commands */
  std::vector<Command> commands;

  {
    Command c;
    c.name = "init";
    c.description = kInitCommandDescription;
    c.help = kInitCommandHelp;
    c.call = std::bind(&zdbtool_init, std::placeholders::_1);
    commands.emplace_back(std::move(c));
  }

  /* print help */
  if (flags.isSet("version")) {
    std::cerr <<
        StringUtil::format(
            "zdbtool $0\n"
            "Part of the libzdb project (https://libzdb.org)\n"
            "Copyright (c) 2016, Paul Asmuth. All rights reserved.\n\n",
            "0.x");

    return 0;
  }

  bool help_requested = false;
  std::string help_topic;

  if (flags.isSetExplicit("help") || cmd_argv.empty()) {
    help_requested = true;
    help_topic = flags.getString("help");
  }
 
  if (cmd_argv.size() > 0 && cmd_argv[0] == "help") {
    help_requested = true;
    help_topic = cmd_argv.size() > 1 ? cmd_argv[1] : "";
  }

  if (help_requested && help_topic.empty()) {
    std::cerr <<
        "Usage: $ zdbtool [OPTIONS] [COMMAND] [COMMAND_OPTIONS]\n"
        "   -v, --verbose             Run in verbose mode\n"
        "   -?, --help <topic>        Display a command's help text and exit\n"
        "   -V, --version             Display the version of this binary and exit\n"
        "\n"
        "Commands:\n";

    for (const auto& c : commands) {
      //stdout_os->printf("   %-26.26s", c.name.c_str());
      //stdout_os->printf("%-80.80s\n", c.description.c_str());
    }

    std::cerr << "\nSee 'zdbtool help <command>' for information about a specific command.\n";
    return 0;
  }

  if (help_requested && !help_topic.empty()) {
    const auto& cmd = std::find_if(
        commands.begin(),
        commands.end(),
        [&help_topic] (const auto& c) { return c.name == help_topic; });

    if (cmd == commands.end()) {
      std::cerr << StringUtil::format("No manual entry for zdbtool '$0'\n", help_topic);
      return 1;
    } else {
      std::cerr << cmd->help;
      return 0;
    }
  }

  /* execute command */
  const auto& cmd = std::find_if(
      commands.begin(),
      commands.end(),
      [&cmd_argv] (const auto& c) { return c.name == cmd_argv.front(); });

  if (cmd == commands.end()) {
    std::cerr << StringUtil::format(
        "ERROR: '$0' is not a zdbtool command. See 'zdbtool --help'.\n",
        cmd_argv[0]);

    return 1;
  }

  cmd_argv.erase(cmd_argv.begin());
  auto rc = cmd->call(cmd_argv);

  if (!rc.isSuccess()) {
    std::cerr << StringUtil::format("ERROR: $0\n", rc.getMessage());
  }

  return rc.isSuccess() ? 0 : 1;
}

