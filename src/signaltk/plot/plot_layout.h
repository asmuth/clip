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

static const int kCharWidth = 6.0f; // FIXPAUL make configurable

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
