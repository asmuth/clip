/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_DRAWABLE_H
#define _FNORDMETRIC_DRAWABLE_H
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include "seriesdefinition.h"
#include "rendertarget.h"
#include "drawables/domain.h"

namespace fnordmetric {

class Drawable {
public:

  enum kPositions {
    TOP = 0,
    RIGHT = 1,
    BOTTOM = 2,
    LEFT = 3
  };

  static const int kNumTicks = 6; // FIXPAUL make configurable;
  static const int kTickLength = 5; // FIXPAUL make configurable

  Drawable() :
      width_(800),
      height_(400),
      padding_top_(0),
      padding_right_(0),
      padding_bottom_(0),
      padding_left_(0) {
    show_axis_[TOP] = true;
    show_axis_[RIGHT] = true;
    show_axis_[BOTTOM] = true;
    show_axis_[LEFT] = true;
  }

  void addSeries(SeriesDefinition* series) {
    series_.push_back(series);
  }

  virtual void draw(ChartRenderTarget* target) {
    /* calculate axis paddings */
    if (show_axis_[TOP]) {
      padding_top_ += 50.0f; // FIXPAUL
    }

    if (show_axis_[RIGHT]) {
      padding_right_ += 50.0f; // FIXPAUL
    }

    if (show_axis_[BOTTOM]) {
      padding_bottom_ += 50.0f; // FIXPAUL
    }

    if (show_axis_[LEFT]) {
      padding_left_ += 50.0f; // FIXPAUL
    }

    /* calculate inner viewport size */
    inner_width_ = width_ - (padding_left_ + padding_right_);
    inner_height_ = height_ - (padding_top_ + padding_bottom_);
  }

protected:

  void drawLeftAxis(ChartRenderTarget* target, Domain* domain) {
    target->beginGroup("axis left");
    target->drawLine(
        padding_left_,
        padding_top_,
        padding_left_,
        padding_top_ + inner_height_,
        "stroke");

    for (int i=0; i < kNumTicks; i++) {
      auto tick = (double) i / (kNumTicks - 1);
      auto tick_y = padding_top_ + inner_height_ * tick;

      target->drawLine(
          padding_left_,
          tick_y,
          padding_left_ - kTickLength,
          tick_y,
          "tick");
    }

    target->finishGroup();
  }

  void drawBottomAxis(ChartRenderTarget* target, Domain* domain) {
    target->beginGroup("axis bottom");
    target->drawLine(
        padding_left_,
        padding_top_ + inner_height_,
        padding_left_ + inner_width_,
        padding_top_ + inner_height_,
        "stroke");
    target->finishGroup();
  }

  const std::vector<SeriesDefinition*>& getSeries() const {
    return series_;
  }

  int width_;
  int height_;
  int padding_top_;
  int padding_right_;
  int padding_bottom_;
  int padding_left_;
  bool show_axis_[4];
  double inner_width_;
  double inner_height_;
private:
  std::vector<SeriesDefinition*> series_;
};

}
#endif
