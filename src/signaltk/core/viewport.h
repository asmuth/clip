/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_UI_VIEWPORT_H
#define _libstx_UI_VIEWPORT_H
#include <tuple>

namespace signaltk {

class Viewport {
public:

  Viewport(
      double width,
      double height) :
      Viewport(width, height, std::tuple<double, double, double, double>(0,0,0,0)) {}

  Viewport(
      double width,
      double height,
      const std::tuple<double, double, double, double>& padding) :
      width_(width),
      height_(height),
      padding_(padding) {}

  double innerWidth() const {
    return width_ - paddingRight() - paddingLeft();
  }

  double innerHeight() const {
    return height_ - paddingTop() - paddingBottom();
  }

  double paddingTop() const {
    return std::get<0>(padding_);
  }

  double paddingRight() const {
    return std::get<1>(padding_);
  }

  double paddingBottom() const {
    return std::get<2>(padding_);
  }

  double paddingLeft() const {
    return std::get<3>(padding_);
  }

  const std::tuple<double, double, double, double>& padding() {
    return padding_;
  }

  void setPadding(const std::tuple<double, double, double, double>& padding) {
    padding_ = padding;
  }

  void setPaddingTop(double val) {
    std::get<0>(padding_) = val;
  }

  void setPaddingRight(double val) {
    std::get<1>(padding_) = val;
  }

  void setPaddingBottom(double val) {
    std::get<2>(padding_) = val;
  }

  void setPaddingLeft(double val) {
    std::get<3>(padding_) = val;
  }

  double width_;
  double height_;
  std::tuple<double, double, double, double> padding_;
};

}
#endif
