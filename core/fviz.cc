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
#include "fviz.h"
#include "element_factory.h"
#include "layout.h"
#include "sexpr_parser.h"
#include "graphics/layer.h"
#include "graphics/layer_svg.h"
#include "graphics/layer_pixmap.h"
#include "utils/fileutil.h"
#include "core/environment.h"
#include "elements/fill.h"
#include "elements/text.h"
#include "elements/chart/axis.h"
#include "elements/chart/layout.h"
#include "elements/chart/points.h"

#include <iostream>
#include <string.h>

using namespace std::placeholders;
using namespace fviz;

struct fviz_s {
  Environment env;
  ElementMap elements;
  ExprStorage expr;
  std::string error;
};

fviz_t* fviz_init() {
  auto ctx = std::make_unique<fviz_t>();
  auto elems = &ctx->env.element_map;
  element_bind(elems, "fill", bind(elements::fill::build, _1, _2, _3));
  element_bind(elems, "text", bind(elements::text::build, _1, _2, _3));
  element_bind(elems, "chart/axis-top", bind(elements::chart::axis::build, _1, _2, _3));
  element_bind(elems, "chart/axis-right", bind(elements::chart::axis::build, _1, _2, _3));
  element_bind(elems, "chart/axis-bottom", bind(elements::chart::axis::build, _1, _2, _3));
  element_bind(elems, "chart/axis-left", bind(elements::chart::axis::build, _1, _2, _3));
  element_bind(elems, "chart/layout", bind(elements::chart::layout::build, _1, _2, _3));
  element_bind(elems, "chart/points", bind(elements::chart::points::build, _1, _2, _3));
  return ctx.release();
}

const char* fviz_geterror(const fviz_t* ctx) {
  return ctx->error.c_str();
}

void fviz_seterrf(fviz_t* ctx, const std::string& err) {
  ctx->error = err;
}

template <typename... T>
void fviz_seterrf(fviz_t* ctx, const std::string& err, T... args) {
  ctx->error = StringUtil::format(err, args...);
}

void fviz_seterr(fviz_t* ctx, const ReturnCode& err) {
  ctx->error = err.getMessage();
}

int fviz_configure(
    fviz_t* ctx,
    const char* config) {
  if (auto rc = expr_parse(config, strlen(config), &ctx->expr); !rc) {
    fviz_seterrf(
        ctx,
        "invalid element specification: $0",
        rc.getMessage());

    return ERROR;
  }

  if (auto rc = environment_setup_defaults(&ctx->env); !rc.isSuccess()) {
    return rc;
  }

  return OK;
}

int fviz_configure_file(
    fviz_t* ctx,
    const char* path) {
  std::string config_path(path);
  std::string config_buf;

  if (auto rc = read_file(config_path, &config_buf); !rc) {
    fviz_seterr(ctx, rc);
    return rc;
  }

  return fviz_configure(ctx, config_buf.c_str());
}

int fviz_render_to(fviz_t* ctx, void* backend) {
  auto layer = static_cast<Layer*>(backend);

  LayoutInfo layout;
  layout.content_box = Rectangle(0, 0, layer->width, layer->height);

  std::vector<ElementRef> roots;
  auto rc = try_chain({
    [&] { return element_build_all(ctx->env, ctx->expr.get(), &roots); },
    [&] () -> ReturnCode {
      for (const auto& e : roots) {
        if (auto rc = e->draw(layout, layer); !rc) {
          return rc;
        }
      }

      return OK;
    },
    [&] { return layer_submit(layer); },
  });

  fviz_seterr(ctx, rc);
  return rc;
}

int fviz_render_file(fviz_t* ctx, const char* path, const char* fmt) {
  std::string format = fmt;

  if (format == "svg")
    return fviz_render_svg_file(ctx, path);

  if (format == "png")
    return fviz_render_png_file(ctx, path);

  fviz_seterrf(ctx, "invalid output format: $0", format);
  return ERROR;
}

int fviz_render_svg_file(fviz_t* ctx, const char* path) {
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
    fviz_seterr(ctx, rc);
    return rc;
  }

  return fviz_render_to(ctx, layer.get());
}

int fviz_render_png_file(fviz_t* ctx, const char* path) {
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
    fviz_seterr(ctx, rc);
    return rc;
  }

  return fviz_render_to(ctx, layer.get());
}

