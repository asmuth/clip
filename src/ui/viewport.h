/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UI_VIEWPORT_H
#define _FNORDMETRIC_UI_VIEWPORT_H

namespace fnordmetric {
namespace ui {

class Viewport {
public:

  Viewport(
      int width,
      int height) :
      Viewport(width, height, std::tuple<int, int, int, int>(0,0,0,0)) {}

  Viewport(
      int width,
      int height,
      const std::tuple<int, int, int, int>& padding) :
      width_(width),
      height_(height),
      padding_(padding) {}

  int innerWidth() const {
    return width_ - paddingRight() - paddingLeft();
  }

  int innerHeight() const {
    return height_ - paddingTop() - paddingBottom();
  }

  int paddingTop() const {
    return std::get<0>(padding_);
  }

  int paddingRight() const {
    return std::get<1>(padding_);
  }

  int paddingBottom() const {
    return std::get<2>(padding_);
  }

  int paddingLeft() const {
    return std::get<3>(padding_);
  }

  void setPadding(const std::tuple<int, int, int, int>& padding) {
    padding_ = padding;
  }

protected:
  int width_;
  int height_;
  std::tuple<int, int, int, int> padding_;
};

}
}
#endif
