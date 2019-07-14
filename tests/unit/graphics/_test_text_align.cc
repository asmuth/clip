/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
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

