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
#include "elements/text.h"
#include "elements/plot.h"
#include "elements/plot/areas.h"
#include "elements/plot/axis.h"
#include "elements/plot/bars.h"
#include "elements/plot/grid.h"
#include "elements/plot/labels.h"
#include "elements/chart/legend.h"
#include "elements/plot/lines.h"
#include "elements/chart/linechart.h"
#include "elements/plot/points.h"
#include "elements/chart/scatterplot.h"
#include "elements/layout/box.h"

#include <iostream>
#include <string.h>

using namespace std::placeholders;
using namespace fviz;

struct fviz_s {
  Environment env;
  ElementMap elements;
  ExprStorage expr;
  ReturnCode error;
};

fviz_t* fviz_init() {
  auto ctx = std::make_unique<fviz_t>();
  auto elems = &ctx->env.element_map;
  element_bind(elems, "text", bind(elements::text::build, _1, _2, _3));
  element_bind(elems, "plot/axis-top", bind(elements::plot::axis::build, _1, _2, _3));
  element_bind(elems, "plot/axis-right", bind(elements::plot::axis::build, _1, _2, _3));
  element_bind(elems, "plot/axis-bottom", bind(elements::plot::axis::build, _1, _2, _3));
  element_bind(elems, "plot/axis-left", bind(elements::plot::axis::build, _1, _2, _3));
  element_bind(elems, "plot", bind(elements::plot::build, _1, _2, _3));
  element_bind(elems, "plot/areas", bind(elements::plot::areas::build, _1, _2, _3));
  element_bind(elems, "plot/bars", bind(elements::plot::bars::build, _1, _2, _3));
  element_bind(elems, "plot/grid", bind(elements::plot::grid::build, _1, _2, _3));
  element_bind(elems, "plot/labels", bind(elements::plot::labels::build, _1, _2, _3));
  element_bind(elems, "chart/legend", bind(elements::chart::legend::build, _1, _2, _3));
  element_bind(elems, "plot/lines", bind(elements::plot::lines::build, _1, _2, _3));
  element_bind(elems, "chart/linechart", bind(elements::chart::linechart::build, _1, _2, _3));
  element_bind(elems, "plot/points", bind(elements::plot::points::build, _1, _2, _3));
  element_bind(elems, "chart/scatterplot", bind(elements::chart::scatterplot::build, _1, _2, _3));
  element_bind(elems, "layout/box", bind(elements::layout::box::build, _1, _2, _3));
  return ctx.release();
}

const char* fviz_geterror(const fviz_t* ctx) {
  return ctx->error.message.c_str();
}

void fviz_printerror(const fviz_t* ctx) {
  error_print(ctx->error, std::cerr);
}

void fviz_seterr(fviz_t* ctx, const ReturnCode& err) {
  ctx->error = err;
}

int fviz_configure(
    fviz_t* ctx,
    const char* config) {
  if (auto rc = expr_parse(config, strlen(config), &ctx->expr); !rc) {
    fviz_seterr(ctx, rc);
    return ERROR;
  }

  if (auto rc = environment_setup_defaults(&ctx->env); !rc) {
    fviz_seterr(ctx, rc);
    return rc;
  }

  if (auto rc = environment_configure(&ctx->env, ctx->expr.get()); !rc) {
    fviz_seterr(ctx, rc);
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

  fviz_seterr(ctx, errorf(ERROR, "invalid output format: {}", format));
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

  if (!rc) {
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

  if (!rc) {
    fviz_seterr(ctx, rc);
    return rc;
  }

  return fviz_render_to(ctx, layer.get());
}

