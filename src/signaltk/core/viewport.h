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

namespace signaltk {

class Viewport {
public:

  Viewport(
      uint32_t width,
      uint32_t height) :
      Viewport(width, height, std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>(0,0,0,0)) {}

  Viewport(
      uint32_t width,
      uint32_t height,
      const std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>& padding) :
      width_(width),
      height_(height),
      padding_(padding) {}

  uint32_t innerWidth() const {
    return width_ - paddingRight() - paddingLeft();
  }

  uint32_t innerHeight() const {
    return height_ - paddingTop() - paddingBottom();
  }

  uint32_t paddingTop() const {
    return std::get<0>(padding_);
  }

  uint32_t paddingRight() const {
    return std::get<1>(padding_);
  }

  uint32_t paddingBottom() const {
    return std::get<2>(padding_);
  }

  uint32_t paddingLeft() const {
    return std::get<3>(padding_);
  }

  const std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>& padding() {
    return padding_;
  }

  void setPadding(const std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>& padding) {
    padding_ = padding;
  }

  void setPaddingTop(uint32_t val) {
    std::get<0>(padding_) = val;
  }

  void setPaddingRight(uint32_t val) {
    std::get<1>(padding_) = val;
  }

  void setPaddingBottom(uint32_t val) {
    std::get<2>(padding_) = val;
  }

  void setPaddingLeft(uint32_t val) {
    std::get<3>(padding_) = val;
  }

  uint32_t width_;
  uint32_t height_;
  std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> padding_;
};

}
#endif
