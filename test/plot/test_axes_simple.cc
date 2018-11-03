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

using namespace signaltk;
using namespace signaltk::chart;

int main(int argc, char** argv) {
  Layer target(1000, 600);
  target.clear(Colour{1, 1, 1, 1});

  double padding = 20;

  // draw left axis
  {
    AxisDefinition axis;
    axis.label_placement = AxisDefinition::LABELS_RIGHT;
    axis.addTick(0.0);
    axis.addTick(0.2);
    axis.addTick(0.4);
    axis.addTick(0.6);
    axis.addTick(0.8);
    axis.addTick(1.0);
    renderAxisVertical(axis, padding, padding, target.height - padding, &target);
  }

  // draw right axis
  {
    AxisDefinition axis;
    axis.label_placement = AxisDefinition::LABELS_LEFT;
    axis.addTick(0.0);
    axis.addTick(0.2);
    axis.addTick(0.4);
    axis.addTick(0.6);
    axis.addTick(0.8);
    axis.addTick(1.0);
    renderAxisVertical(axis, target.width - padding, padding, target.height - padding, &target);
  }

  //axis_left->setTitle("my axis");
  //axis_left->addTick(0.0);
  //axis_left->addTick(0.2);
  //axis_left->addTick(0.4);
  //axis_left->addTick(0.6);
  //axis_left->addTick(0.8);
  //axis_left->addTick(1.0);
  //axis_left->addLabel(0.0, "0");
  //axis_left->addLabel(0.2, "1");
  //axis_left->addLabel(0.4, "2");
  //axis_left->addLabel(0.6, "3");
  //axis_left->addLabel(0.8, "4");
  //axis_left->addLabel(1.0, "5");


  target.writePNG(std::string(argv[0]) + ".png");
}

