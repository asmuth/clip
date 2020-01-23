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
#include <assert.h>
#include <string.h>
#include "image.h"

namespace clip {

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
  return clip::getPixelSize(pixel_format_);
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

Color Image::getPixel(size_t x, size_t y) {
  return getPixel(y * width_ + x);
}

Color Image::getPixel(size_t idx) {
  assert(idx < width_ * height_);

  auto pixel_size = getPixelSize();
  return decodePixel(
      pixel_format_,
      static_cast<char*>(pixmap_) + idx * pixel_size,
      pixel_size);
}

void Image::setPixel(size_t x, size_t y, const Color& color) {
  return setPixel(y * width_ + x, color);
}

void Image::setPixel(size_t idx, const Color& color) {
  assert(idx < width_ * height_);

  auto pixel_size = getPixelSize();
  encodePixel(
      pixel_format_,
      color,
      static_cast<char*>(pixmap_) + idx * pixel_size,
      pixel_size);
}

void Image::clear(const Color& color) {
  auto pixel_size = getPixelSize();
  for (size_t i = 0; i < width_ * height_; ++i) {
    encodePixel(
        pixel_format_,
        color,
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
    const Color& color,
    char* data,
    size_t size) {
  assert(size >= 4);
  data[0] = color[0] * 255;
  data[1] = color[1] * 255;
  data[2] = color[2] * 255;
  data[3] = color[3] * 255;
}

void encodePixel(
    PixelFormat pixel_format,
    const Color& color,
    char* data,
    size_t size) {
  switch (pixel_format) {

    case PixelFormat::RGBA8:
      return encodePixel_RGBA8(color, data, size);

    default:
      return;

  }
}

Color decodePixel_RGBA8(
    char* data,
    size_t size) {
  assert(size >= 4);
  Color c;
  c[0] = data[0] / 255.0f;
  c[1] = data[1] / 255.0f;
  c[2] = data[2] / 255.0f;
  c[3] = data[3] / 255.0f;
  return c;
}

Color decodePixel(
    PixelFormat pixel_format,
    char* data,
    size_t size) {
  switch (pixel_format) {

    case PixelFormat::RGBA8:
      return decodePixel_RGBA8(data, size);

    default:
      return Color{};

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

} // namespace clip

