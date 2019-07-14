/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2017 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include "color.h"

namespace fviz {

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

} // namespace fviz

