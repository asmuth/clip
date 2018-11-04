/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "layer.h"
#include <signaltk/util/stringutil.h>
#include <signaltk/codec/png.h>

namespace signaltk {

Layer::Layer(
    uint32_t w,
    uint32_t h,
    uint32_t dpi_ /* = 300 */) :
    width(w),
    height(h),
    dpi(dpi_),
    pixmap(PixelFormat::RGBA8, w, h),
    text_shaper(dpi),
    rasterizer(&pixmap, dpi) {}

Layer::~Layer() {}

Status Layer::writeToFile(const std::string& path) {
  if (StringUtil::endsWith(path, ".png")) {
    return pngWriteImageFile(pixmap, path);
  }

  return ERROR_INVALID_ARGUMENT;
}

Status Layer::loadFromFile(const std::string& path) const {
  return ERROR_INVALID_ARGUMENT;
}

void Layer::clear(const Colour& c) {
  pixmap.clear(c);
}

} // namespace signaltk

