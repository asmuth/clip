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
  static const int kAxisLabelHeight = 35.0f; // FIXPAUL make configurable
  static const int kAxisLabelWidth = 50.0f; // FIXPAUL make configurable
  static const int kAxisTitleLength = 20.0f; // FIXPAUL make configurable

  Drawable() :
      width_(1024),
      height_(500),
      padding_top_(0),
      padding_right_(0),
      padding_bottom_(0),
      padding_left_(0) {
    show_axis_[TOP] = true;
    show_axis_[RIGHT] = true;
    show_axis_[BOTTOM] = true;
    show_axis_[LEFT] = true;
    axis_title_[TOP] = "top axis";
    axis_title_[RIGHT] = "right axis";
    axis_title_[BOTTOM] = "bottom axis";
    axis_title_[LEFT] = "left axis";
  }

  void addSeries(SeriesDefinition* series) {
    series_.push_back(series);
  }

  virtual void draw(ChartRenderTarget* target) {
    /* calculate axis paddings */
    if (show_axis_[TOP]) {
      padding_top_ += kAxisLabelHeight;
      if (axis_title_[TOP].size() > 0) {
        padding_top_ += kAxisTitleLength;
      }
    }

    if (show_axis_[RIGHT]) {
      padding_right_ += kAxisLabelWidth;
      if (axis_title_[RIGHT].size() > 0) {
        padding_right_ += kAxisTitleLength;
      }
    }

    if (show_axis_[BOTTOM]) {
      padding_bottom_ += kAxisLabelHeight;
      if (axis_title_[BOTTOM].size() > 0) {
        padding_bottom_ += kAxisTitleLength;
      }
    }

    if (show_axis_[LEFT]) {
      padding_left_ += kAxisLabelWidth;
      if (axis_title_[LEFT].size() > 0) {
        padding_left_ += kAxisTitleLength;
      }
    }

    /* calculate inner viewport size */
    inner_width_ = width_ - (padding_left_ + padding_right_);
    inner_height_ = height_ - (padding_top_ + padding_bottom_);
  }

protected:

  void drawLeftAxis(ChartRenderTarget* target, Domain* domain) {
    target->beginGroup("axis left");

    /* draw stroke */
    target->drawLine(
        padding_left_,
        padding_top_,
        padding_left_,
        padding_top_ + inner_height_,
        "stroke");

    /* draw title */
    if (axis_title_[LEFT].size() > 0) {
      target->drawText(
          axis_title_[LEFT],
          padding_left_ - kAxisLabelWidth - kAxisTitleLength,
          padding_top_ + inner_height_ * 0.5f,
          "middle",
          "text-before-edge",
          "title",
          270);
    }

    /* draw ticks */
    for (int i=0; i < kNumTicks; i++) {
      auto tick = (double) i / (kNumTicks - 1);
      auto tick_y = padding_top_ + inner_height_ * tick;

      target->drawLine(
          padding_left_,
          tick_y,
          padding_left_ - kTickLength,
          tick_y,
          "tick");

      target->drawText(
          "Tick",
          padding_left_ - (kTickLength * 2),
          tick_y,
          "end",
          "middle",
          "label");
    }

    target->finishGroup();
  }

  void drawRightAxis(ChartRenderTarget* target, Domain* domain) {
    target->beginGroup("axis right");

    /* draw stroke */
    target->drawLine(
        width_ - padding_right_,
        padding_top_,
        width_ - padding_right_,
        padding_top_ + inner_height_,
        "stroke");

    /* draw title */
    if (axis_title_[RIGHT].size() > 0) {
      target->drawText(
          axis_title_[RIGHT],
          (width_ - padding_right_) + kAxisLabelWidth + kAxisTitleLength * 0.5f,
          padding_top_ + inner_height_ * 0.5f,
          "middle",
          "middle",
          "title",
          270);
    }

    /* draw ticks */
    for (int i=0; i < kNumTicks; i++) {
      auto tick = (double) i / (kNumTicks - 1);
      auto tick_y = padding_top_ + inner_height_ * tick;

      target->drawLine(
          (width_ - padding_right_),
          tick_y,
          (width_ - padding_right_) + kTickLength,
          tick_y,
          "tick");

      target->drawText(
          "Tick",
          (width_ - padding_right_) + (kTickLength * 2),
          tick_y,
          "start",
          "middle",
          "label");
    }

    target->finishGroup();
  }

  void drawTopAxis(ChartRenderTarget* target, Domain* domain) {
    target->beginGroup("axis top");

    /* draw stroke */
    target->drawLine(
        padding_left_,
        padding_top_,
        padding_left_ + inner_width_,
        padding_top_,
        "stroke");

    /* draw title */
    if (axis_title_[TOP].size() > 0) {
      target->drawText(
          axis_title_[TOP],
          padding_left_ + inner_width_ * 0.5f,
          padding_top_ - kAxisLabelHeight - kAxisTitleLength,
          "middle",
          "text-before-edge",
          "title");
    }

    /* draw ticks */
    for (int i=0; i < kNumTicks; i++) {
      auto tick = (double) i / (kNumTicks - 1);
      auto tick_x = padding_left_ + inner_width_ * tick;

      target->drawLine(
          tick_x,
          padding_top_,
          tick_x,
          padding_top_ - kTickLength,
          "tick");

      target->drawText(
          "Tick",
          tick_x,
          padding_top_ - kAxisLabelHeight * 0.5f,
          "middle",
          "middle",
          "label");
    }

    target->finishGroup();
  }

  void drawBottomAxis(ChartRenderTarget* target, Domain* domain) {
    target->beginGroup("axis bottom");

    /* draw stroke */
    target->drawLine(
        padding_left_,
        padding_top_ + inner_height_,
        padding_left_ + inner_width_,
        padding_top_ + inner_height_,
        "stroke");

    /* draw title */
    if (axis_title_[BOTTOM].size() > 0) {
      target->drawText(
          axis_title_[BOTTOM],
          padding_left_ + inner_width_ * 0.5f,
          padding_top_ + inner_height_ + kAxisLabelHeight + kAxisTitleLength,
          "middle",
          "no-change",
          "title");
    }

    /* draw ticks */
    for (int i=0; i < kNumTicks; i++) {
      auto tick = (double) i / (kNumTicks - 1);
      auto tick_x = padding_left_ + inner_width_ * tick;

      target->drawLine(
          tick_x,
          padding_top_ + inner_height_,
          tick_x,
          padding_top_ + inner_height_ + kTickLength,
          "tick");

      target->drawText(
          "Tick",
          tick_x,
          padding_top_ + inner_height_ + kAxisLabelHeight * 0.5f,
          "middle",
          "middle",
          "label");
    }

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
  std::string axis_title_[4];
  double inner_width_;
  double inner_height_;
private:
  std::vector<SeriesDefinition*> series_;
};

}
#endif
