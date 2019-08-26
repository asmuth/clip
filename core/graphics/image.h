/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include "color.h"

namespace clip {

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

  Color getPixel(size_t x, size_t y);
  Color getPixel(size_t idx);
  void setPixel(size_t x, size_t y, const Color& color);
  void setPixel(size_t idx, const Color& color);
  void clear(const Color& color);

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
    const Color& color,
    char* data,
    size_t size);

Color decodePixel(
    PixelFormat pixel_format,
    char* data,
    size_t size);

} // namespace clip

