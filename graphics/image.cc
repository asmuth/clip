/**
 * This file is part of the "plotfx" project
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
#include <assert.h>
#include <string.h>
#include "image.h"

namespace signaltk {

Image::Image(
    PixelFormat pixel_format,
    size_t width,
    size_t height) :
    pixel_format_(pixel_format),
    width_(width),
    height_(height),
    pixmap_(nullptr) {
  auto pixmap_size = width_ * height_ * getPixelSize();
  pixmap_ = malloc(pixmap_size);
  if (!pixmap_) {
    throw std::bad_alloc(); // FIXME?
  }
}

Image::Image(
    Image&& other) :
    pixel_format_(other.pixel_format_),
    width_(other.width_),
    height_(other.height_),
    pixmap_(other.pixmap_) {
  other.width_ = 0;
  other.height_ = 0;
  other.pixmap_ = nullptr;
}

Image::~Image() {
  if (pixmap_) {
    free(pixmap_);
  }
}

PixelFormat Image::getPixelFormat() const {
  return pixel_format_;
}

size_t Image::getPixelCount() const {
  return width_ * height_;
}

size_t Image::getPixelSize() const {
  return signaltk::getPixelSize(pixel_format_);
}

size_t Image::getWidth() const {
  return width_;
}

size_t Image::getHeight() const {
  return height_;
}

const void* Image::getData() const {
  return pixmap_;
}

void* Image::getData() {
  return pixmap_;
}

size_t Image::getDataSize() const {
  return width_ * height_ * getPixelSize();
}

Colour Image::getPixel(size_t x, size_t y) {
  return getPixel(y * width_ + x);
}

Colour Image::getPixel(size_t idx) {
  assert(idx < width_ * height_);

  auto pixel_size = getPixelSize();
  return decodePixel(
      pixel_format_,
      static_cast<char*>(pixmap_) + idx * pixel_size,
      pixel_size);
}

void Image::setPixel(size_t x, size_t y, const Colour& colour) {
  return setPixel(y * width_ + x, colour);
}

void Image::setPixel(size_t idx, const Colour& colour) {
  assert(idx < width_ * height_);

  auto pixel_size = getPixelSize();
  encodePixel(
      pixel_format_,
      colour,
      static_cast<char*>(pixmap_) + idx * pixel_size,
      pixel_size);
}

void Image::clear(const Colour& colour) {
  auto pixel_size = getPixelSize();
  for (size_t i = 0; i < width_ * height_; ++i) {
    encodePixel(
        pixel_format_,
        colour,
        static_cast<char*>(pixmap_) + i * pixel_size,
        pixel_size);
  }
}

size_t getPixelSize(PixelFormat pixel_format) {
  switch (pixel_format) {

    case PixelFormat::RGB8:
      return 3;

    case PixelFormat::RGBA8:
      return 4;

    default:
      return 0;

  }
}

void encodePixel_RGBA8(
    const Colour& colour,
    char* data,
    size_t size) {
  assert(size >= 4);
  data[0] = colour[0] * 255;
  data[1] = colour[1] * 255;
  data[2] = colour[2] * 255;
  data[3] = colour[3] * 255;
}

void encodePixel(
    PixelFormat pixel_format,
    const Colour& colour,
    char* data,
    size_t size) {
  switch (pixel_format) {

    case PixelFormat::RGBA8:
      return encodePixel_RGBA8(colour, data, size);

    default:
      return;

  }
}

Colour decodePixel_RGBA8(
    char* data,
    size_t size) {
  assert(size >= 4);
  Colour c;
  c[0] = data[0] / 255.0f;
  c[1] = data[1] / 255.0f;
  c[2] = data[2] / 255.0f;
  c[3] = data[3] / 255.0f;
  return c;
}

Colour decodePixel(
    PixelFormat pixel_format,
    char* data,
    size_t size) {
  switch (pixel_format) {

    case PixelFormat::RGBA8:
      return decodePixel_RGBA8(data, size);

    default:
      return Colour{};

  }
}

Image convertImage_RGB8_RGBA8(const Image& img) {
  Image newimg(PixelFormat::RGBA8, img.getWidth(), img.getHeight());

  auto src = ((const unsigned char*) img.getData());
  auto dst = ((unsigned char*) newimg.getData());
  for (size_t i = 0; i < img.getWidth() * img.getHeight(); ++i) {
    memcpy(dst + i * 4, src + i * 3, 3); 
    dst[i * 4 + 3] = 0xff;
  }

  return newimg;
}

Image convertImage_RGB8A_RGB8(const Image& img) {
  Image newimg(PixelFormat::RGB8, img.getWidth(), img.getHeight());

  auto src = ((const unsigned char*) img.getData());
  auto dst = ((unsigned char*) newimg.getData());
  for (size_t i = 0; i < img.getWidth() * img.getHeight(); ++i) {
    memcpy(dst + i * 3, src + i * 4, 3); 
  }

  return newimg;
}

} // namespace signaltk

