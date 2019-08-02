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
#include "layer_pixmap.h"
#include "rasterize.h"

namespace fviz {

ReturnCode layer_bind_img(
    double width,
    double height,
    double dpi,
    Measure font_size,
    const Color& background_color,
    std::function<Status (const unsigned char* data, size_t len)> submit,
    LayerRef* layer) {
  auto raster = std::make_shared<Rasterizer>(width, height, dpi);
  raster->clear(background_color);

  LayerRef l(new Layer());
  l->width = width,
  l->height = height,
  l->dpi = dpi,
  l->font_size = font_size,
  l->apply = [submit, raster] (auto op) {
    return std::visit([submit, raster] (auto&& op) {
      using T = std::decay_t<decltype(op)>;
      if constexpr (std::is_same_v<T, layer_ops::BrushStrokeOp>)
        return raster->strokePath(op);
      if constexpr (std::is_same_v<T, layer_ops::BrushFillOp>)
        return raster->fillPath(op);
      if constexpr (std::is_same_v<T, layer_ops::TextSpanOp>)
        return raster->drawText(op);
      if constexpr (std::is_same_v<T, layer_ops::SubmitOp>)
        return submit(raster->data(), raster->size());
      else
        return ERROR;
    }, op);
  };

  *layer = std::move(l);
  return OK;
}

ReturnCode layer_bind_png(
    double width,
    double height,
    double dpi,
    Measure font_size,
    const Color& background_color,
    std::function<Status (const std::string&)> submit,
    LayerRef* layer) {
  auto raster = std::make_shared<Rasterizer>(width, height, dpi);
  raster->clear(background_color);

  LayerRef l(new Layer());
  l->width = width,
  l->height = height,
  l->dpi = dpi,
  l->font_size = font_size,
  l->apply = [submit, raster] (auto op) {
    return std::visit([submit, raster] (auto&& op) {
      using T = std::decay_t<decltype(op)>;
      if constexpr (std::is_same_v<T, layer_ops::BrushStrokeOp>)
        return raster->strokePath(op);
      if constexpr (std::is_same_v<T, layer_ops::BrushFillOp>)
        return raster->fillPath(op);
      if constexpr (std::is_same_v<T, layer_ops::TextSpanOp>)
        return raster->drawText(op);
      if constexpr (std::is_same_v<T, layer_ops::SubmitOp>)
        return submit(raster->to_png());
      else
        return ERROR;
    }, op);
  };

  *layer = std::move(l);
  return OK;
}

} // namespace fviz

