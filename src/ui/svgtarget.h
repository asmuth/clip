/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_SVGTARGET_H
#define _FNORDMETRIC_SVGTARGET_H
#include <stdlib.h>
#include <vector>
#include <string>
#include "rendertarget.h"
#include "styles/style_default.h"

namespace fnordmetric {
namespace ui {

class SVGTarget : public RenderTarget {
public:

  SVGTarget() : indent_(0) {}

#define appendLine(...) { \
    for(int __i = 0; __i < indent_ * 2; ++__i) printf(" "); \
    printf(__VA_ARGS__); }

  void beginChart(
      int width,
      int height,
      const std::string& class_name) {
    appendLine(
        "<svg width='%i' height='%i' class='%s'>\n",
        width,
        height,
        class_name.c_str());

    indent_ ++;
    appendLine("<style type='text/css'>\n");
    appendLine("<![CDATA[%s  ]]>\n", kStyleSheetDefault.c_str());
    appendLine("</style>\n");
  }

  void finishChart() {
    indent_--;
    appendLine("</svg>\n");
  }

  void drawRect(
      double x,
      double y,
      double width,
      double height,
      const std::string& class_name) override {
    appendLine(
        "<rect x='%f' y='%f' width='%f' height='%f' class='%s'></rect>\n",
        x,
        y,
        width,
        height,
        class_name.c_str());
  }

  void drawLine(
      double x1,
      double y1,
      double x2,
      double y2,
      const std::string& class_name) override {
    appendLine(
        "<line x1='%f' y1='%f' x2='%f' y2='%f' class='%s' />\n",
        x1,
        y1,
        x2,
        y2,
        class_name.c_str());
  }

  void drawText(
      const std::string& text,
      double x,
      double y,
      const std::string& halign,
      const std::string& valign,
      const std::string& class_name,
      double rotate = 0.0f) override {
    char transform[256];

    if (rotate == 0.0f) {
      *transform = 0;
    } else {
      snprintf(
          transform,
          sizeof(transform),
          "transform='translate(0,0) rotate(%f %f %f)'",
          rotate,
          x,
          y);
    }

    appendLine(
        "<text x='%f' y='%f' style='text-anchor:%s; dominant-baseline:%s;' "
            "class='%s' %s>%s</text>\n",
        x,
        y,
        halign.c_str(),
        valign.c_str(),
        class_name.c_str(),
        transform,
        text.c_str());
  }

  void beginGroup(const std::string& class_name) override {
    appendLine("<g class='%s'>\n", class_name.c_str());
    indent_++;
  }

  void finishGroup() override {
    indent_--;
    appendLine("</g>\n");
  }

protected:

  int indent_;
};


}
}
#endif
