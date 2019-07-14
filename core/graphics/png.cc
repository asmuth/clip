/**
 * This file is part of the "fviz" project
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
#include <unistd.h>
#include <iostream>
#include <png.h>
#include <string.h>
#include "png.h"
#include "utils/file.h"
#include "utils/fileutil.h"

namespace fviz {

Status pngWriteImageFile(
    const Image& image,
    const std::string& filename) {
  int color_type;
  int bit_depth;
  switch (image.getPixelFormat()) {
    case PixelFormat::RGB8:
      bit_depth = 8;
      color_type = PNG_COLOR_TYPE_RGB;
      break;
    case PixelFormat::RGBA8:
      bit_depth = 8;
      color_type = PNG_COLOR_TYPE_RGB_ALPHA;
      break;
    default:
      return ERROR;
  }

  FILE* file = fopen(filename.c_str(), "wb");

  auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    fclose(file);
    return ERROR;
  }

  auto png_info = png_create_info_struct(png);
  if (!png_info) {
    fclose(file);
    png_destroy_write_struct(&png, NULL);
    return ERROR;
  }

  png_init_io(png, file);

  png_set_IHDR(
      png,
      png_info,
      image.getWidth(),
      image.getHeight(),
      bit_depth,
      color_type,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_BASE,
      PNG_FILTER_TYPE_BASE);

  png_write_info(png, png_info);

  std::vector<uint8_t> buf(sizeof(png_bytep) * image.getHeight());
  for (size_t i = 0; i < image.getHeight(); i++) {
    auto row = ((char *) image.getData()) + i * image.getPixelSize() * image.getWidth();
    ((png_bytepp) buf.data())[i] = (png_byte *) row;
  }

  png_write_image(png, (png_bytepp) buf.data());

  png_write_end(png, NULL);
  png_destroy_info_struct(png, &png_info);
  png_destroy_write_struct(&png, NULL);
  fclose(file);

  return OK;
}

} // namespace fviz

