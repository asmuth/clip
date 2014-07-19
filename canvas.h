/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CANVAS_H
#define _FNORDMETRIC_CANVAS_H
#include <vector>
#include <string>

namespace fnordmetric {
class SeriesDefinition;
namespace ui {

class Canvas {
public:

/*
  enum kPositions {
    TOP = 0,
    RIGHT = 1,
    BOTTOM = 2,
    LEFT = 3
  };


  static const int kNumTicks = 6; // FIXPAUL make configurable;
  static const int kTickLength = 5; // FIXPAUL make configurable
  static const int kAxisLabelHeight = 35.0f; // FIXPAUL make configurable
  static const int kAxisLabelWidth = 50.0f; // FIXPAUL make configurable
  static const int kAxisTitleLength = 20.0f; // FIXPAUL make configurable
*/

  Canvas() :
      width_(1024),
      height_(500),
      padding_top_(0),
      padding_right_(0),
      padding_bottom_(0),
      padding_left_(0) {}


  /**
   * Render the contents of this canvas as a scalabale vector graphic and return
   * as string
   */
   std::string renderSVG();

protected:

  int width_;
  int height_;
  int padding_top_;
  int padding_right_;
  int padding_bottom_;
  int padding_left_;
  bool show_axis_[4];
  std::string axis_title_[4];
  double inner_width_;
  double inner_height_;
private:
  std::vector<SeriesDefinition*> series_;
};

}
}
#endif
