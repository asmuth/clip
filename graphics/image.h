/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2017 Paul Asmuth
 *
 * signaltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include "colour.h"

namespace signaltk {

enum class PixelFormat {
  RGB8, RGBA8
};

class Image {
public:

  Image(
      PixelFormat pixel_format,
      size_t width,
      size_t height);

  ~Image();
  Image(const Image& other) = delete;
  Image(Image&& other);
  Image& operator=(const Image& other) = delete;

  PixelFormat getPixelFormat() const;
  size_t getPixelSize() const;
  size_t getPixelCount() const;
  size_t getWidth() const;
  size_t getHeight() const;
  const void* getData() const;
  void* getData();
  size_t getDataSize() const;

  Colour getPixel(size_t x, size_t y);
  Colour getPixel(size_t idx);
  void setPixel(size_t x, size_t y, const Colour& colour);
  void setPixel(size_t idx, const Colour& colour);
  void clear(const Colour& colour);

protected:
  PixelFormat pixel_format_;
  size_t width_;
  size_t height_;
  void* pixmap_;
};

Image convertImage_RGB8_RGBA8(const Image& img);
Image convertImage_RGBA8_RGB8(const Image& img);

size_t getPixelSize(PixelFormat pixel_format);

void encodePixel(
    PixelFormat pixel_format,
    const Colour& colour,
    char* data,
    size_t size);

Colour decodePixel(
    PixelFormat pixel_format,
    char* data,
    size_t size);

} // namespace signaltk

