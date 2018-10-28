/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_CANVAS_H
#define _libstx_CANVAS_H
#include <memory>
#include <vector>
#include <string>
#include <tuple>
#include <signaltk/core/layer.h>
#include <signaltk/core/viewport.h>
#include "axes.h"
#include "griddefinition.h"
#include "legenddefinition.h"

namespace signaltk {
namespace chart {

static const int kAxisPadding = 0; // FIXPAUL make configurable
static const int kTickLength = 5; // FIXPAUL make configurable
static const int kAxisLabelHeight = 25; // FIXPAUL make configurable
static const int kAxisLabelWidth = 50; // FIXPAUL make configurable
static const int kAxisTitleLength = 20; // FIXPAUL make configurable
static const int kCharWidth = 6.0f; // FIXPAUL make configurable

/**
 * Render the axes
 */
void renderAxes(
    const Layer& input,
    Layer* target,
    Viewport* viewport);

/**
 * Render a top axis
 *
 * @param target the render target
 * @param axis the axis definition
 * @param padding the padding state
 * @param top the top padding for this axis
 */
void renderTopAxis(
    Layer* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int top);

/**
 * Render a right axis
 *
 * @param target the render target
 * @param axis the axis definition
 * @param padding the padding state
 * @param right the right padding for this axis
 */
void renderRightAxis(
    Layer* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int right);

/**
 * Render a bottom axis
 *
 * @param target the render target
 * @param axis the axis definition
 * @param padding the padding state
 * @param bottom the bottom padding for this axis
 */
void renderBottomAxis(
    Layer* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int bottom);

/**
 * Render a left axis
 *
 * @param target the render target
 * @param axis the axis definition
 * @param padding the padding state
 * @param left the left padding for this axis
 */
void renderLeftAxis(
    Layer* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int left);

/**
 * Render the grid
 */
void renderGrid(
    const GridDefinition& grid_config,
    const Viewport& viewport,
    Layer* target);

}
}
#endif
