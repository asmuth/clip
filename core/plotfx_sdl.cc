/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "plotfx_sdl.h"
#include "utils/return_code.h"
#include "graphics/layer.h"
#include "graphics/layer_pixmap.h"
#include "SDL2/SDL.h"

using namespace plotfx;

static void blit(
    const unsigned char* image_data,
    size_t image_width,
    size_t image_height,
    SDL_Surface* output_surface) {
  auto image = SDL_CreateRGBSurfaceWithFormatFrom(
      (void*) image_data,
      image_width,
      image_height,
      32,
      4 * image_width,
      SDL_PIXELFORMAT_ARGB32);

   SDL_BlitSurface(image, NULL, output_surface, NULL);
   SDL_FreeSurface(image);
}

/*
int plotfx_render_sdl2(plotfx_t* ctx, SDL_Surface* surface) {
  const auto& doc = static_cast<const Context*>(ctx)->document;
  if (!doc) {
    ctx_seterrf(ctx, "no configuration loaded");
    return ERROR;
  }

  auto w = surface->w;
  auto h = surface->h;

  LayerRef layer;
  auto rc = layer_bind_img(
      w,
      h,
      doc->dpi,
      doc->font_size,
      doc->background_color,
      [w, h, surface] (const unsigned char* data, size_t data_len) {
        blit(data, w, h, surface);
        return OK;
      },
      &layer);

  if (!rc.isSuccess()) {
    return rc;
  }

  if (auto rc = document_render_to(*doc, layer.get()); !rc.isSuccess()) {
    return rc;
  }

  return OK;
}
*/

