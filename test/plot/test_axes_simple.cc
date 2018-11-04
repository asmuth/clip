/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "signaltk/plot/axes.h"
#include "../unittest.h"

using namespace signaltk;
using namespace signaltk::chart;

int main(int argc, char** argv) {
  Layer target(1200, 800);
  target.clear(Colour{1, 1, 1, 1});

  double padding = 80;

  // draw left axis
  {
    AxisDefinition axis;
    axis.label_placement = AxisDefinition::LABELS_LEFT;
    axis.addTick(0.0);
    axis.addTick(0.2);
    axis.addTick(0.4);
    axis.addTick(0.6);
    axis.addTick(0.8);
    axis.addTick(1.0);
    axis.addLabel(0.0, "asd");
    axis.addLabel(0.2, "blah");
    axis.addLabel(0.4, "xxx");
    axis.addLabel(0.6, "d");
    axis.addLabel(0.8, "e");
    axis.addLabel(1.0, "f");
    renderAxisVertical(axis, padding, padding, target.height - padding, &target);
  }

  // draw right axis
  {
    AxisDefinition axis;
    axis.label_placement = AxisDefinition::LABELS_RIGHT;
    axis.addTick(0.0);
    axis.addTick(0.2);
    axis.addTick(0.4);
    axis.addTick(0.6);
    axis.addTick(0.8);
    axis.addTick(1.0);
    axis.addLabel(0.0, "a");
    axis.addLabel(0.2, "b");
    axis.addLabel(0.4, "c");
    axis.addLabel(0.6, "d");
    axis.addLabel(0.8, "e");
    axis.addLabel(1.0, "f");
    renderAxisVertical(axis, target.width - padding, padding, target.height - padding, &target);
  }

  // draw top axis
  {
    AxisDefinition axis;
    axis.label_placement = AxisDefinition::LABELS_TOP;
    axis.addTick(0.0);
    axis.addTick(0.2);
    axis.addTick(0.4);
    axis.addTick(0.6);
    axis.addTick(0.8);
    axis.addTick(1.0);
    axis.addLabel(0.0, "a");
    axis.addLabel(0.2, "b");
    axis.addLabel(0.4, "c");
    axis.addLabel(0.6, "d");
    axis.addLabel(0.8, "e");
    axis.addLabel(1.0, "f");
    renderAxisHorizontal(axis, padding, padding, target.width - padding, &target);
  }

  // draw bottom axis
  {
    AxisDefinition axis;
    axis.label_placement = AxisDefinition::LABELS_BOTTOM;
    axis.addTick(0.0);
    axis.addTick(0.2);
    axis.addTick(0.4);
    axis.addTick(0.6);
    axis.addTick(0.8);
    axis.addTick(1.0);
    axis.addLabel(0.0, "a");
    axis.addLabel(0.2, "b");
    axis.addLabel(0.4, "c");
    axis.addLabel(0.6, "d");
    axis.addLabel(0.8, "e");
    axis.addLabel(1.0, "f");
    renderAxisHorizontal(axis, target.height - padding, padding, target.width - padding, &target);
  }

  CHECK_RC(target.writeToFile(std::string(argv[0]) + ".png"));
}

