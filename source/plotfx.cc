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
#include "config_helpers.h"
#include "element_factory.h"
#include "plist/plist_parser.h"
#include "graphics/layer.h"
#include "graphics/layer_svg.h"
#include "graphics/layer_pixmap.h"
#include "utils/fileutil.h"
#include "common/environment.h"

#include <iostream>
#include <fstream>
#include <sstream>
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
  std::stringstream config_buf;
  {
    std::ifstream config_file(path);
    if (!config_file.is_open()) {
      plotfx_seterrf(ctx, StringUtil::format("file not found: $0", path));
      return ERROR;
    }

    config_buf << config_file.rdbuf();
  }

  return plotfx_configure(ctx, config_buf.str().c_str());
}

int plotfx_render_to(plotfx_t* ctx, void* backend) {
  auto& plist = ctx->plist;
  auto layer = static_cast<Layer*>(backend);

  std::vector<ElementRef> roots;
  for (size_t i = 0; i < plist.size(); ++i) {
    const auto& elem_name = plist[i].name;

    if (!plist::is_map(plist[i])) {
      continue;
    }

    const auto& elem_config = plist[i].next.get();

    ElementRef elem;
    auto rc = buildElement(
        elem_name,
        *elem_config,
        ctx->env,
        &elem);

    if (!rc) {
      plotfx_seterr(ctx, rc);
      return rc;
    }

    roots.emplace_back(std::move(elem));
  }

  auto bounding_box = Rectangle(0, 0, layer->width, layer->height);
  auto content_box = layout_margin_box(bounding_box, 20, 20, 20, 20);

  std::vector<Rectangle> element_boxes;
  for (const auto& e : roots) {
    LayoutInfo layout;
    layout.bounding_box = bounding_box;
    layout.constraint = {true, true};
    layout.content_box = content_box;

    if (auto rc = e->layout(*layer, &layout); !rc.isSuccess()) {
      plotfx_seterr(ctx, rc);
      return rc;
    }

    content_box = layout.content_box;
    element_boxes.emplace_back(layout.element_box);
  }

  for (size_t i = 0; i < roots.size(); ++i) {
    const auto& element = roots[i];
    LayoutInfo layout;
    layout.bounding_box = bounding_box;
    layout.constraint = {true, true};
    layout.content_box = content_box;
    layout.element_box = element_boxes[i];

    if (auto rc = element->draw(layout, layer); !rc.isSuccess()) {
      plotfx_seterr(ctx, rc);
      return rc;
    }
  }

  if (auto rc = layer_submit(layer); !rc.isSuccess()) {
    plotfx_seterr(ctx, rc);
    return rc;
  }

  return OK;
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

