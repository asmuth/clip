/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth, Laura Schlimmer
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "fviz.h"
#include "fviz_config.h"
#include "utils/flagparser.h"
#include "return_code.h"
#include "utils/stringutil.h"
#include "core/environment.h"

using namespace fviz;

void printError(const ReturnCode& rc) {
  std::cerr << fmt::format("ERROR: {}", rc.message) << std::endl;
}

int main(int argc, const char** argv) {
  FlagParser flag_parser;

  std::string flag_in;
  flag_parser.defineString("in", false, &flag_in);

  std::string flag_out;
  flag_parser.defineString("out", false, &flag_out);

  std::string flag_out_fmt;
  flag_parser.defineString("outfmt", false, &flag_out_fmt);

  bool flag_help = false;
  flag_parser.defineSwitch("help", &flag_help);

  bool flag_version = false;
  flag_parser.defineSwitch("version", &flag_version);

  bool flag_debug = true;
  flag_parser.defineSwitch("debug", &flag_debug);

  bool flag_font_defaults = true;
  flag_parser.defineSwitch("font-defaults", &flag_font_defaults);

  std::vector<std::string> flag_font_load;
  flag_parser.defineStringV("font-load", &flag_font_load);

  {
    auto rc = flag_parser.parseArgv(argc - 1, argv + 1);
    if (!rc) {
      std::cerr << "ERROR: " << rc.message << std::endl;
      return -1;
    }
  }

  if (flag_version) {
    std::cerr <<
        fmt::format(
            "fviz {}\n"
            "Part of the fviz project (https://fviz.org)\n"
            "Copyright (c) 2019, Paul Asmuth, Laura Schlimmer.\n"
            "All rights reserved.\n\n",
            FVIZ_VERSION);

    return 0;
  }

  if (flag_help) {
    std::cerr <<
        "Usage: $ fviz [OPTIONS]\n"
        "   --help                Display this help text and exit\n"
        "   --version             Display the version of this binary and exit\n"
        "\n"
        "Commands:\n";


    std::cerr << "\nSee 'fviz help <command>' for information about a specific subcommand.\n";
    return 0;
  }

  if (flag_in.empty()) {
    std::cerr << "Need an input file (--in)\n";
    return 1;
  }

  if (flag_out.empty()) {
    std::cerr << "Need an output file (--out)\n";
    return 1;
  }

  std::string fmt = flag_out_fmt;
  if (fmt.empty()) {
    if (StringUtil::endsWith(flag_out, ".svg")) { fmt = "svg"; }
    if (StringUtil::endsWith(flag_out, ".png")) { fmt = "png"; }
  }

  fviz_t* ctx = fviz_init();
  if (!ctx) {
    std::cerr << "ERROR: error while initializing fviz" << std::endl;
    return EXIT_FAILURE;
  }

  Environment* env = static_cast<Environment*>(fviz_env(ctx));
  env->font_defaults = flag_font_defaults;
  env->font_load = flag_font_load;

  if (!fviz_configure_file(ctx, flag_in.c_str())) {
    fviz_printerror(ctx);
    return EXIT_FAILURE;
  }

  if (!fviz_render_file(ctx, flag_out.c_str(), fmt.c_str())) {
    fviz_printerror(ctx);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
