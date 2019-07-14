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
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>

namespace fviz {

enum class PathCommand {
  MOVE_TO,
  LINE_TO,
  QUADRATIC_CURVE_TO,
  CUBIC_CURVE_TO,
  ARC_TO,
  CLOSE
};

const size_t kPathMaxCoefficients = 6;

struct PathData {
  PathCommand command;
  double coefficients[kPathMaxCoefficients];
  double operator[](size_t idx) const;
};

struct Path {
public:

  Path();
  Path(const PathData* data, size_t size);

  void moveTo(double x, double y);
  void lineTo(double x, double y);
  void quadraticCurveTo(double cx, double cy, double x, double y);
  void cubicCurveTo(double c1x, double c1y, double c2x, double c2y, double x, double y);
  void arcTo(double cx, double cy, double r, double a1, double a2);
  void closePath();

  const PathData& operator[](size_t idx) const;
  PathData& operator[](size_t idx);

  const PathData* data() const;
  PathData* data();
  const PathData* begin() const;
  PathData* begin();
  const PathData* end() const;
  PathData* end();
  size_t size() const;
  bool empty() const;

protected:
  std::vector<PathData> data_;
};

} // namespace fviz

