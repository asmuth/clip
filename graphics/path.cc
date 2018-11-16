/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth
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
#include <iostream>
#include "path.h"

namespace signaltk {

double PathData::operator[](size_t idx) const {
  return coefficients[idx];
}

Path::Path() {}

Path::Path(const PathData* data, size_t size) {
  data_.reserve(size);

  for (size_t i = 0; i < size; ++i) {
    data_.emplace_back(data[i]);
  }
}

void Path::moveTo(double x, double y) {
  PathData d;
  d.command = PathCommand::MOVE_TO;
  d.coefficients[0] = x;
  d.coefficients[1] = y;
  data_.emplace_back(std::move(d));
}

void Path::lineTo(double x, double y) {
  PathData d;
  d.command = PathCommand::LINE_TO;
  d.coefficients[0] = x;
  d.coefficients[1] = y;
  data_.emplace_back(std::move(d));
}

void Path::quadraticCurveTo(double cx, double cy, double x, double y) {
  PathData d;
  d.command = PathCommand::QUADRATIC_CURVE_TO;
  d.coefficients[0] = cx;
  d.coefficients[1] = cy;
  d.coefficients[2] = x;
  d.coefficients[3] = y;
  data_.emplace_back(std::move(d));
}

void Path::cubicCurveTo(double c1x, double c1y, double c2x, double c2y, double x, double y) {
  PathData d;
  d.command = PathCommand::CUBIC_CURVE_TO;
  d.coefficients[0] = c1x;
  d.coefficients[1] = c1y;
  d.coefficients[2] = c2x;
  d.coefficients[3] = c2y;
  d.coefficients[4] = x;
  d.coefficients[5] = y;
  data_.emplace_back(std::move(d));
}

void Path::arcTo(double cx, double cy, double r, double a1, double a2) {
  PathData d;
  d.command = PathCommand::ARC_TO;
  d.coefficients[0] = cx;
  d.coefficients[1] = cy;
  d.coefficients[2] = r;
  d.coefficients[3] = a1;
  d.coefficients[4] = a2;
  data_.emplace_back(std::move(d));
}

void Path::closePath() {
  PathData d;
  d.command = PathCommand::CLOSE;
  data_.emplace_back(std::move(d));
}

const PathData& Path::operator[](size_t idx) const {
  return data_[idx];
}

PathData& Path::operator[](size_t idx) {
  return data_[idx];
}

const PathData* Path::data() const {
  return &*data_.data();
}

PathData* Path::data() {
  return &*data_.data();
}

const PathData* Path::begin() const {
  return &*data_.begin();
}

PathData* Path::begin() {
  return &*data_.begin();
}

const PathData* Path::end() const {
  return &*data_.end();
}

PathData* Path::end() {
  return &*data_.end();
}

size_t Path::size() const {
  return data_.size();
}

bool Path::empty() const {
  return data_.size() == 0;
}

} // namespace signaltk
