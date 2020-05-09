/**
 * This file is part of the "clip" project
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
#include <iterator>
#include <string>
#include <getopt.h>

#include "config.h"
#include "context.h"
#include "utils/flagparser.h"
#include "return_code.h"
#include "utils/stringutil.h"
#include "graphics/export_svg.h"
#include "eval.h"
#include "fileutil.h"

using namespace clip;

void printError(const ReturnCode& rc) {
  std::cerr << fmt::format("ERROR: {}", rc.message) << std::endl;
}

int main(int argc, char** argv) {
  FlagList flags;

  std::string flag_in;
  flags_add_string(&flags, 'i', "in", &flag_in);

  std::string flag_out;
  flags_add_string(&flags, 'e', "out", &flag_out);

  bool flag_stdin = false;
  flags_add_switch(&flags, 0, "stdin", &flag_stdin);

  bool flag_stdout = false;
  flags_add_switch(&flags, 0, "stdout", &flag_stdout);

  std::string flag_format;
  flags_add_string(&flags, 'f', "format", &flag_format);

  bool flag_help = false;
  flags_add_switch(&flags, 'h', "help", &flag_help);

  bool flag_version = false;
  flags_add_switch(&flags, 'v', "version", &flag_version);

  bool flag_debug = true;
  flags_add_switch(&flags, 'd', "debug", &flag_debug);

  std::vector<std::string> flag_font_load;
  flags_add_stringv(&flags, 0, "font-load", &flag_font_load);

  std::vector<std::string> args;
  if (auto rc = flags_parse(flags, argc, argv, &args); !rc) {
    std::cerr << "ERROR: " << rc.message << std::endl;
    return EXIT_FAILURE;
  }

  if (flag_version) {
    std::cerr <<
        fmt::format(
            "clip {}\n"
            "Part of the clip project (https://clip.org)\n"
            "Copyright (c) 2019, Paul Asmuth, Laura Schlimmer.\n"
            "All rights reserved.\n\n",
            CLIP_VERSION);

    return 0;
  }

  if (flag_help) {
    std::cerr <<
        "Usage: $ clip [OPTIONS]\n"
        "  --in <path>               Path to the input file\n"
        "  --out <path>              Path to the output file\n"
        "  --stdin                   Read the input file from stdin\n"
        "  --stdout                  Write the output file from sdout\n"
        "  --format <format>         Output format. If no format is given, it is inferred from the\n"
        "                            filename. Valid values: 'png', 'svg'\n"
        "  --font-defaults <bool>    Enable or disable default font loading. Default is enabled.\n"
        "                            Valid values: 'on' and 'off'\n"
        "  --font-load <path>        Add a font file to the default font list\n"
        "  --debug                   Run in debug mode\n"
        "  --help                    Display this help text and exit\n"
        "  --version                 Display the version of this binary and exit\n"
        "\n"
        "Examples:\n"
        "  $ clip --in my_chart.clp --out my_chart.svg\n";

    return 0;
  }

  /* check if the input flags are valid */
  if (flag_in.empty() && !flag_stdin) {
    std::cerr << "Need an input file (--in)\n";
    return 1;
  }

  if (!flag_in.empty() && flag_stdin) {
    std::cerr
        << "Can't read from an input file (--in) and stdin (--stdin) at the "
        << "same time\n";

    return 1;
  }

  if (flag_out.empty() && !flag_stdout) {
    std::cerr << "Need an output file (--out)\n";
    return 1;
  }

  if (!flag_out.empty() && flag_stdout) {
    std::cerr
        << "Can't write to an output file (--out) and stdout (--stdout) at the "
        << "same time\n";

    return 1;
  }

  /* figure out which output format the user wants */
  auto output_format = clip::OutputFormat::SVG;
  if (flag_format.empty()) {
    if (StringUtil::endsWith(flag_out, ".svg"))
      output_format = OutputFormat::SVG;
    if (StringUtil::endsWith(flag_out, ".png"))
      output_format = OutputFormat::PNG;
  } else if (flag_format == "svg") {
    output_format = OutputFormat::SVG;
  } else if (flag_format == "png") {
    output_format = OutputFormat::PNG;
  } else {
    std::cerr
        << "ERROR: invalid output format (--format). valid values are "
        << "'svg' and 'png'"
        << std::endl;

    return EXIT_FAILURE;
  }

  /* set up the context */
  Context ctx;
  ctx.font_load = flag_font_load;

  if (auto rc = context_setup_defaults(&ctx); !rc) {
    error_print(rc, std::cerr);
    return EXIT_FAILURE;
  }

  /* read the input file */
  std::string input;
  if (flag_stdin) {
    std::istreambuf_iterator<char> begin(std::cin), end;
    input = std::string(begin, end);
  } else {
    if (auto rc = read_file(flag_in, &input); !rc) {
      fmt::print(
          stderr,
          "ERROR: unable to read input file ({}): {}\n",
          flag_in,
          rc.message);

      return EXIT_FAILURE;
    }
  }

  /* evaluate the input commands */
  if (auto rc = clip::eval(&ctx, input); !rc) {
    error_print(rc, std::cerr);
    return EXIT_FAILURE;
  }

  /* write the output file */
  std::string output_buffer;
  ReturnCode export_rc;
  switch (output_format) {
    case OutputFormat::SVG:
      export_rc = export_svg(ctx.layer.get(), &output_buffer);
      break;
    //case OutputFormat::PNG:
    //  export_rc = page_export_png(page, drawlist, output_buffer);
    //  break;
  }

  if (!export_rc) {
    error_print(export_rc, std::cerr);
    return EXIT_FAILURE;
  }

  if (flag_stdout) {
    std::cout << output_buffer;
  } else {
    FileUtil::write(
        flag_out,
        Buffer(output_buffer.data(), output_buffer.size()));
  }

  return EXIT_SUCCESS;
}
