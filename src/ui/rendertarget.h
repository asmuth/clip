/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_RENDERTARGET_H
#define _FNORDMETRIC_RENDERTARGET_H
#include <stdlib.h>
#include <vector>
#include <string>
#include <fnordmetric/query/executable.h>
#include <fnordmetric/query/svalue.h>

namespace fnordmetric {
namespace ui {

class RenderTarget {
public:

  virtual void beginChart(
      int width,
      int height,
      const std::string& class_name) = 0;

  virtual void finishChart() = 0;

  virtual void beginGroup(const std::string& class_name) = 0;
  virtual void finishGroup() = 0;

  virtual void drawLine(
      double x1,
      double y1,
      double x2,
      double y2,
      const std::string& class_name) = 0;

  virtual void drawText(
      const std::string& text,
      double x,
      double y,
      const std::string& halign,
      const std::string& valign,
      const std::string& class_name,
      double rotate = 0.0f) = 0;

  virtual void drawPoint(
      double x,
      double y,
      const std::string& point_type,
      double point_size,
      const std::string& color,
      const std::string& class_name = "") = 0;

  virtual void drawRect(
      double x,
      double y,
      double width,
      double height,
      const std::string& color,
      const std::string& class_name = "") = 0;

  virtual void drawPath(
      const std::vector<std::pair<double, double>>& points,
      const std::string& line_style,
      double line_width,
      bool smooth,
      const std::string& color,
      const std::string& class_name = "") = 0;

};


}
}
#endif
