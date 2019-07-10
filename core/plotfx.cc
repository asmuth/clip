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
#include "plotfx.h"
//#include "config_helpers.h"
//#include "element_factory.h"
#include "sexpr_parser.h"
#include "graphics/layer.h"
#include "graphics/layer_svg.h"
#include "graphics/layer_pixmap.h"
#include "utils/fileutil.h"
//#include "core/environment.h"
//#include "elements/plot.h"

#include <iostream>
#include <string.h>

using namespace plotfx;

struct plotfx_s {
  Environment env;
  PropertyList plist;
  std::string error;
};

plotfx_t* plotfx_init() {
  auto ctx = std::make_unique<plotfx_t>();
  return ctx.release();
}

const char* plotfx_geterror(const plotfx_t* ctx) {
  return ctx->error.c_str();
}

void plotfx_seterrf(plotfx_t* ctx, const std::string& err) {
  ctx->error = err;
}

template <typename... T>
void plotfx_seterrf(plotfx_t* ctx, const std::string& err, T... args) {
  ctx->error = StringUtil::format(err, args...);
}

void plotfx_seterr(plotfx_t* ctx, const ReturnCode& err) {
  ctx->error = err.getMessage();
}

int plotfx_configure(
    plotfx_t* ctx,
    const char* config) {
  auto& plist = ctx->plist;

  plist::PropertyListParser plist_parser(config, strlen(config));
  if (!plist_parser.parse(&plist)) {
    plotfx_seterrf(
        ctx,
        "invalid element specification: $0",
        plist_parser.get_error());

    return ERROR;
  }

  if (auto rc = environment_configure(plist, &ctx->env); !rc) {
    plotfx_seterr(ctx, rc);
    return rc;
  }

  return OK;
}

int plotfx_configure_file(
    plotfx_t* ctx,
    const char* path) {
  std::string config_path(path);
  std::string config_buf;

  if (auto rc = read_file(config_path, &config_buf); !rc) {
    plotfx_seterr(ctx, rc);
    return rc;
  }

  return plotfx_configure(ctx, config_buf.c_str());
}

int plotfx_render_to(plotfx_t* ctx, void* backend) {
  auto layer = static_cast<Layer*>(backend);

  LayoutInfo layout;
  layout.bounding_box = Rectangle(0, 0, layer->width, layer->height);
  layout.content_box = layout.bounding_box;
  layout.inner_box = layout.bounding_box;

  plot::PlotConfig root;
  root.margins = {from_px(20), from_px(20), from_px(20), from_px(20)};

  auto rc = try_chain({
    [&] { return plot::configure(ctx->plist, ctx->env, &root); },
    [&] { return plot::draw(root, layout, layer); },
    [&] { return layer_submit(layer); },
  });

  plotfx_seterr(ctx, rc);
  return rc;
}

int plotfx_render_file(plotfx_t* ctx, const char* path, const char* fmt) {
  std::string format = fmt;

  if (format == "svg")
    return plotfx_render_svg_file(ctx, path);

  if (format == "png")
    return plotfx_render_png_file(ctx, path);

  plotfx_seterrf(ctx, "invalid output format: $0", format);
  return ERROR;
}

int plotfx_render_svg_file(plotfx_t* ctx, const char* path) {
  std::string path_str(path);

  LayerRef layer;
  auto rc = layer_bind_svg(
      ctx->env.screen_width,
      ctx->env.screen_height,
      ctx->env.dpi,
      ctx->env.font_size,
      ctx->env.background_color,
      [path_str] (auto svg) {
        FileUtil::write(path_str, Buffer(svg.data(), svg.size()));
        return OK;
      },
      &layer);

  if (!rc.isSuccess()) {
    plotfx_seterr(ctx, rc);
    return rc;
  }

  return plotfx_render_to(ctx, layer.get());
}

int plotfx_render_png_file(plotfx_t* ctx, const char* path) {
  std::string path_str(path);

  LayerRef layer;
  auto rc = layer_bind_png(
      ctx->env.screen_width,
      ctx->env.screen_height,
      ctx->env.dpi,
      ctx->env.font_size,
      ctx->env.background_color,
      [path_str] (auto svg) {
        FileUtil::write(path_str, Buffer(svg.data(), svg.size()));
        return OK;
      },
      &layer);

  if (!rc.isSuccess()) {
    plotfx_seterr(ctx, rc);
    return rc;
  }

  return plotfx_render_to(ctx, layer.get());
}

