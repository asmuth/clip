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
#include "fviz_sdl.h"
#include "return_code.h"
#include "graphics/layer.h"
#include "graphics/layer_pixmap.h"
#include "SDL2/SDL.h"

using namespace fviz;

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
int fviz_render_sdl2(fviz_t* ctx, SDL_Surface* surface) {
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

  if (!rc) {
    return rc;
  }

  if (auto rc = document_render_to(*doc, layer.get()); !rc) {
    return rc;
  }

  return OK;
}
*/

