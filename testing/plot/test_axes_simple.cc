/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "elements/plot/axes.h"
#include "elements/plot/plot_layout.h"
#include "elements/context.h"
#include "../unittest.h"

using namespace signaltk;
using namespace signaltk::chart;

int main(int argc, char** argv) {
  Context ctx {
    .frame = Layer(1200, 800),
  };

  ctx.frame.clear(Colour{1, 1, 1, 1});

  CHECK_RC(plot_render(&ctx));


  CHECK_RC(ctx.frame.writeToFile(std::string(argv[0]) + ".png"));
}

