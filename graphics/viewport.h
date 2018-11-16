/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
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
#ifndef _libstx_UI_VIEWPORT_H
#define _libstx_UI_VIEWPORT_H
#include <tuple>

namespace plotfx {

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
