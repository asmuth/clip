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

  CHECK_RC(plot_add(&ctx));

  CHECK_RC(plot_axis_add(&ctx, AxisPosition::LEFT));
  CHECK_RC(plot_axis_addtick(&ctx, 0.0f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.2f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.4f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.6f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.8f));
  CHECK_RC(plot_axis_addtick(&ctx, 1.0f));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.0f, "1"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.2f, "2"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.4f, "3"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.6f, "4"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.8f, "5"));
  CHECK_RC(plot_axis_addlabel(&ctx, 1.0f, "6"));

  CHECK_RC(plot_axis_add(&ctx, AxisPosition::RIGHT));
  CHECK_RC(plot_axis_addtick(&ctx, 0.0f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.2f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.4f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.6f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.8f));
  CHECK_RC(plot_axis_addtick(&ctx, 1.0f));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.1f, "A"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.3f, "B"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.5f, "C"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.7f, "D"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.9f, "E"));

  CHECK_RC(plot_axis_add(&ctx, AxisPosition::TOP));
  CHECK_RC(plot_axis_addtick(&ctx, 0.0f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.2f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.4f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.6f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.8f));
  CHECK_RC(plot_axis_addtick(&ctx, 1.0f));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.1f, "A"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.3f, "B"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.5f, "C"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.7f, "D"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.9f, "E"));

  CHECK_RC(plot_axis_add(&ctx, AxisPosition::BOTTOM));
  CHECK_RC(plot_axis_addtick(&ctx, 0.0f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.2f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.4f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.6f));
  CHECK_RC(plot_axis_addtick(&ctx, 0.8f));
  CHECK_RC(plot_axis_addtick(&ctx, 1.0f));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.0f, "1"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.2f, "2"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.4f, "3"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.6f, "4"));
  CHECK_RC(plot_axis_addlabel(&ctx, 0.8f, "5"));
  CHECK_RC(plot_axis_addlabel(&ctx, 1.0f, "6"));

  CHECK_RC(plot_render(&ctx));

  CHECK_RC(ctx.frame.writeToFile(std::string(argv[0]) + ".png"));
}

