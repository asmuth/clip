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

void crash() {
  std::cerr << "ERROR: clip crashed; sorry :(" << std::endl;;
  std::cerr << "ERROR: Please report the bug at https://github.com/asmuth/clip/issues" << std::endl;

  auto ex = std::current_exception();
  if (ex) {
    try {
      std::rethrow_exception(ex);
    } catch (const std::exception& e) {
      std::cerr << "ERROR: Exception: " << e.what() << std::endl;
    } catch (...) {
      std::cerr << "ERROR: Foreign exception" << std::endl;
    }
  }

  exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  std::set_terminate(crash);

  FlagList flags;

  std::string flag_export;
  flags_add_string(&flags, 'e', "export", &flag_export);

  std::string flag_format;
  flags_add_string(&flags, 'f', "export-format", &flag_format);

  std::vector<std::string> flag_font_load;
  flags_add_stringv(&flags, 0, "font-load", &flag_font_load);

  bool flag_debug = true;
  flags_add_switch(&flags, 'd', "debug", &flag_debug);

  bool flag_help = false;
  flags_add_switch(&flags, 'h', "help", &flag_help);

  bool flag_version = false;
  flags_add_switch(&flags, 'v', "version", &flag_version);

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

  if (flag_help || args.size() != 1) {
    std::cerr <<
        "Usage: clip [OPTIONS] <file>\n"
        "\n"
        "Export:\n"
        "  -e, --export <path>          Export the image\n"
        "  -f, --export-format <fmt>    Export format. If no format is given, it is\n"
        "                               inferred from the export filename.\n"
        "\n"
        "Fonts:\n"
        "  --font-load <path>           Add a font file to the default font list\n"
        "\n"
        "Other:\n"
        "  -d, --debug                  Run in debug mode\n"
        "  -h, --help                   Display this help text and exit\n"
        "  -v, --version                Display the version of this binary and exit\n"
        "\n"
        "Examples:\n"
        "  $ clip -e my_chart.svg my_chart.clp\n";

    return 0;
  }

  /* figure out which output format the user wants */
  auto output_format = clip::OutputFormat::SVG;
  if (flag_format.empty()) {
    if (StringUtil::endsWith(flag_export, ".svg"))
      output_format = OutputFormat::SVG;
    if (StringUtil::endsWith(flag_export, ".png"))
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
  const auto& input_path = args[0];
  if (input_path == "-") {
    std::istreambuf_iterator<char> begin(std::cin), end;
    input = std::string(begin, end);
  } else {
    if (auto rc = read_file(input_path, &input); !rc) {
      fmt::print(
          stderr,
          "ERROR: unable to read input file ({}): {}\n",
          input_path,
          rc.message);

      return EXIT_FAILURE;
    }
  }

  /* evaluate the input commands */
  if (auto rc = clip::eval(&ctx, input); !rc) {
    error_print(rc, std::cerr);
    return EXIT_FAILURE;
  }

  /* export the context's layers if requested */
  if (!flag_export.empty()) {
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

    if (flag_export == "-") {
      std::cout << output_buffer;
    } else {
      FileUtil::write(
          flag_export,
          Buffer(output_buffer.data(), output_buffer.size()));
    }
  }

  return EXIT_SUCCESS;
}
