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
#pragma once
#include "fviz.h"
#include "SDL2/SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize a new fviz context that will draw to the provided SDL2 surface.
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int fviz_render_sdl2(fviz_t* ctx, SDL_Surface* surface);

#ifdef __cplusplus
} // extern C
#endif

