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
#include "signaltk.h"
#include "elements/element_tree.h"
#include "graphics/layer.h"
#include <utils/flagparser.h>
#include <utils/fileutil.h>
#include <utils/return_code.h>
#include <utils/stringutil.h>

using namespace signaltk;

void printError(const ReturnCode& rc) {
  std::cerr << StringUtil::format("ERROR: $0", rc.getMessage()) << std::endl;
}

int main(int argc, const char** argv) {
  FlagParser flag_parser;

  std::string flag_in;
  flag_parser.defineString("in", true, &flag_in);

  std::string flag_out;
  flag_parser.defineString("out", true, &flag_out);

  bool flag_help;
  flag_parser.defineSwitch("help", &flag_help);

  bool flag_version;
  flag_parser.defineSwitch("version", &flag_version);

  {
    auto rc = flag_parser.parseArgv(argc - 1, argv + 1);
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return -1;
    }
  }

  if (flag_version) {
    std::cerr <<
        StringUtil::format(
            "signaltk $0\n"
            "Part of the FnordMetric project (http://fnordmetric.io)\n"
            "Copyright (c) 2016, Paul Asmuth et al. All rights reserved.\n\n",
            FNORDMETRIC_VERSION);

    return 0;
  }

  if (flag_help) {
    std::cerr <<
        "Usage: $ signaltk [OPTIONS]\n"
        "   --help                Display this help text and exit\n"
        "   --version             Display the version of this binary and exit\n"
        "\n"
        "Commands:\n";


    std::cerr << "\nSee 'signaltk help <command>' for information about a specific subcommand.\n";
    return 0;
  }

  auto spec = FileUtil::read(flag_in).toString(); // FIXME
  signaltk::ElementTree elems;
  if (auto rc = buildElementTree(spec, &elems); !rc.isSuccess()) {
    printError(rc);
    return EXIT_FAILURE;
  }

  Layer frame{1200, 800};
  frame.clear(Colour{1, 1, 1, 1});
  if (auto rc = renderElements(elems, &frame); !rc.isSuccess()) {
    printError(rc);
    return EXIT_FAILURE;
  }

  if (auto rc = frame.writeToFile(flag_out); rc) {
    std::cerr << "ERROR: can't write output file" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
