/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2016 Paul Asmuth, Laura Schlimmer, FnordCorp B.V.
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <regex>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/file.h>
#include "plotfx.h"
#include "common/document.h"
#include "graphics/layer.h"
#include "graphics/layer_svg.h"
#include "graphics/layer_pixmap.h"
#include <utils/flagparser.h>
#include <utils/fileutil.h>
#include <utils/return_code.h>
#include <utils/stringutil.h>

using namespace plotfx;

void printError(const ReturnCode& rc) {
  std::cerr << StringUtil::format("ERROR: $0", rc.getMessage()) << std::endl;
}

int main(int argc, const char** argv) {
  FlagParser flag_parser;

  std::string flag_in;
  flag_parser.defineString("in", true, &flag_in);

  std::string flag_out;
  flag_parser.defineString("out", true, &flag_out);

  std::string flag_out_fmt;
  flag_parser.defineString("outfmt", false, &flag_out_fmt);

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
            "plotfx $0\n"
            "Part of the FnordMetric project (http://fnordmetric.io)\n"
            "Copyright (c) 2016, Paul Asmuth et al. All rights reserved.\n\n",
            FNORDMETRIC_VERSION);

    return 0;
  }

  if (flag_help) {
    std::cerr <<
        "Usage: $ plotfx [OPTIONS]\n"
        "   --help                Display this help text and exit\n"
        "   --version             Display the version of this binary and exit\n"
        "\n"
        "Commands:\n";


    std::cerr << "\nSee 'plotfx help <command>' for information about a specific subcommand.\n";
    return 0;
  }

  auto spec = FileUtil::read(flag_in).toString(); // FIXME
  plotfx::Document doc;
  if (auto rc = setupDocumentDefaults(&doc); !rc.isSuccess()) {
    printError(rc);
    return EXIT_FAILURE;
  }

  if (auto rc = buildDocument(spec, &doc); !rc.isSuccess()) {
    printError(rc);
    return EXIT_FAILURE;
  }

  std::string layer_fmt = flag_out_fmt;
  if (layer_fmt.empty()) {
    if (StringUtil::endsWith(flag_out, ".svg")) { layer_fmt = "svg"; }
    if (StringUtil::endsWith(flag_out, ".png")) { layer_fmt = "png"; }
  }

  LayerRef layer;
  if (layer_fmt == "svg") {
    auto rc = layer_bind_svg(
        to_px(doc.measures, doc.width).value,
        to_px(doc.measures, doc.height).value,
        doc.measures,
        [flag_out] (auto svg) {
          FileUtil::write(flag_out, Buffer(svg.data(), svg.size()));
          return OK;
        },
        &layer);

    if (!rc.isSuccess()) {
      printError(rc);
      return EXIT_FAILURE;
    }
  }

  if (layer_fmt == "png") {
    auto rc = layer_bind_png(
        to_px(doc.measures, doc.width).value,
        to_px(doc.measures, doc.height).value,
        doc.measures,
        doc.background_colour,
        [flag_out] (auto png) {
          FileUtil::write(flag_out, Buffer(png.data(), png.size()));
          return OK;
        },
        &layer);

    if (!rc.isSuccess()) {
      printError(rc);
      return EXIT_FAILURE;
    }
  }

  if (!layer) {
    std::cerr << "ERROR: unknown output format: " << layer_fmt << std::endl;
    return EXIT_FAILURE;
  }

  if (auto rc = renderElements(doc, layer.get()); !rc.isSuccess()) {
    printError(rc);
    return EXIT_FAILURE;
  }

  if (auto rc = layer_submit(layer.get()); !rc.isSuccess()) {
    printError(rc);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
