/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_UI_COLORPALETTE_H
#define _libstx_UI_COLORPALETTE_H
#include <stdlib.h>

namespace stx {
namespace chart {

class ColorPalette {
public:

  ColorPalette(
      const std::vector<std::string>& colors = std::vector<std::string>{
          "color1",
          "color2",
          "color3",
          "color4",
          "color5",
          "color6"}) :
          colors_(colors),
          color_index_(0) {}

  void setNextColor(Series* series) {
    series->setDefaultProperty(
        Series::P_COLOR,
        colors_[color_index_++ % colors_.size()]);
  }

protected:
  std::vector<std::string> colors_;
  size_t color_index_;
};

}
}
#endif
