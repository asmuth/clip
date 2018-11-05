/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "plot/axes.h"
#include "../unittest.h"

using namespace signaltk;
using namespace signaltk::chart;

Status drawTestAt(Layer* l, uint32_t x, uint32_t y, TextStyle s) {
  StrokeStyle ss;
  strokeLine(l, x - 10, y, x + 10, y, ss);
  strokeLine(l, x, y - 10, x, y + 10, ss);
  drawText("Ijsselmeerdijk", x, y, s, l);
}

int main(int argc, char** argv) {
  Layer l(800, 500);
  l.clear(Colour{1, 1, 1, 1});


  {
    TextStyle s;
    s.halign = TextHAlign::LEFT;
    s.valign = TextVAlign::BASELINE;
    drawTestAt(&l, 100,  100, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::CENTER;
    s.valign = TextVAlign::BASELINE;
    drawTestAt(&l, 400,  100, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::RIGHT;
    s.valign = TextVAlign::BASELINE;
    drawTestAt(&l, 700,  100, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::LEFT;
    s.valign = TextVAlign::TOP;
    drawTestAt(&l, 100,  200, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::CENTER;
    s.valign = TextVAlign::TOP;
    drawTestAt(&l, 400,  200, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::RIGHT;
    s.valign = TextVAlign::TOP;
    drawTestAt(&l, 700,  200, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::LEFT;
    s.valign = TextVAlign::MIDDLE;
    drawTestAt(&l, 100,  300, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::CENTER;
    s.valign = TextVAlign::MIDDLE;
    drawTestAt(&l, 400,  300, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::RIGHT;
    s.valign = TextVAlign::MIDDLE;
    drawTestAt(&l, 700,  300, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::LEFT;
    s.valign = TextVAlign::BOTTOM;
    drawTestAt(&l, 100,  400, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::CENTER;
    s.valign = TextVAlign::BOTTOM;
    drawTestAt(&l, 400,  400, s);
  }

  {
    TextStyle s;
    s.halign = TextHAlign::RIGHT;
    s.valign = TextVAlign::BOTTOM;
    drawTestAt(&l, 700,  400, s);
  }

  CHECK_RC(l.writeToFile(std::string(argv[0]) + ".png"));
}

