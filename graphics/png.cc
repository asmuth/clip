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
#include <unistd.h>
#include <iostream>
#include <png.h>
#include <string.h>
#include "png.h"
#include "utils/file.h"
#include "utils/fileutil.h"

namespace signaltk {

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
      return ERROR_INVALID_ARGUMENT;
  }

  FILE* file = fopen(filename.c_str(), "wb");

  auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    fclose(file);
    return ERROR_IO;
  }

  auto png_info = png_create_info_struct(png);
  if (!png_info) {
    fclose(file);
    png_destroy_write_struct(&png, NULL);
    return ERROR_IO;
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

} // namespace signaltk

