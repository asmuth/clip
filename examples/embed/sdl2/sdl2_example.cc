/**
 * Copyright (c) 2019 Paul Asmuth
 * All rights reserved.
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
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <plotfx/plotfx.h>
#include <plotfx/plotfx_sdl.h>

/* The plot configuration that we are going to display */
static const char* kScript = R"(
  scale-x-padding: 0.1;
  scale-y-padding: 0.3;

  x: inline(10,   20,   30,   40,   50,   60,    70,   80,  90,  100);
  y: inline(1.23, 4.32, 3.23, 6.43, 3.45, 12.32, 8.14, 5.2, 3.5, 2.2);

  layer {
    type: lines;
    color: #06c;
  }
)";

int main(int argc, const char** argv) {
  /* Open SDL window */
  SDL_Init(SDL_INIT_VIDEO);

  auto window = SDL_CreateWindow(
      "PlotFX Example",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      1200,
      600,
      SDL_WINDOW_OPENGL);

  if (window == NULL) {
    std::cerr << "ERROR: error while initializing SDL" << std::endl;
    return EXIT_FAILURE;
  }

  /* Setup PlotFX */
  plotfx_t* plot = plotfx_init();
  if (!plot) {
    std::cerr << "ERROR: error while initializing PlotFX" << std::endl;
    return EXIT_FAILURE;
  }

  if (!plotfx_configure(plot, kScript)) {
    std::cerr
        << "ERROR: error while parsing configuration: "
        << plotfx_geterror(plot)
        << std::endl;
    return EXIT_FAILURE;
  }

  /* Render the plot */
  if (!plotfx_render_sdl2(plot, SDL_GetWindowSurface(window))) {
    std::cerr
        << "ERROR: error while rendering: "
        << plotfx_geterror(plot)
        << std::endl;
    return EXIT_FAILURE;
  }

  SDL_UpdateWindowSurface(window);

  /* Wait 5s and exit */
  SDL_Delay(5000);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}

