/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "graphics/brush.h"
#include "graphics/color.h"
#include "graphics/layer.h"
#include "graphics/text.h"
#include "../unittest.h"

using namespace fviz;

Status drawTestAt(Layer* l, uint32_t x, uint32_t y, TextStyle s) {
  StrokeStyle ss;
  strokeLine(l, x - 10, y, x + 10, y, ss);
  strokeLine(l, x, y - 10, x, y + 10, ss);
  drawText("Ijsselmeerdijk", x, y, s, l);
}

int main(int argc, char** argv) {
  Layer l(800, 500);
  l.clear(Color{1, 1, 1, 1});


  {
    TextStyle s;
    //s.halign = TextHAlign::LEFT;
    //s.valign = TextVAlign::BASELINE;
    drawTestAt(&l, 100,  100, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::CENTER;
    //s.valign = TextVAlign::BASELINE;
    drawTestAt(&l, 400,  100, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::RIGHT;
    //s.valign = TextVAlign::BASELINE;
    drawTestAt(&l, 700,  100, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::LEFT;
    //s.valign = TextVAlign::TOP;
    drawTestAt(&l, 100,  200, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::CENTER;
    //s.valign = TextVAlign::TOP;
    drawTestAt(&l, 400,  200, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::RIGHT;
    //s.valign = TextVAlign::TOP;
    drawTestAt(&l, 700,  200, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::LEFT;
    //s.valign = TextVAlign::MIDDLE;
    drawTestAt(&l, 100,  300, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::CENTER;
    //s.valign = TextVAlign::MIDDLE;
    drawTestAt(&l, 400,  300, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::RIGHT;
    //s.valign = TextVAlign::MIDDLE;
    drawTestAt(&l, 700,  300, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::LEFT;
    //s.valign = TextVAlign::BOTTOM;
    drawTestAt(&l, 100,  400, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::CENTER;
    //s.valign = TextVAlign::BOTTOM;
    drawTestAt(&l, 400,  400, s);
  }

  {
    TextStyle s;
    //s.halign = TextHAlign::RIGHT;
    //s.valign = TextVAlign::BOTTOM;
    drawTestAt(&l, 700,  400, s);
  }

  CHECK_RC(l.writeToFile(std::string(argv[0]) + ".png"));
}

