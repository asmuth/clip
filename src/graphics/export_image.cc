/**
 * This file is part of the "clip" project
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
#include "export_image.h"
#include "rasterize.h"

using std::bind;
using namespace std::placeholders;

namespace clip {

struct DrawOp {
  std::function<ReturnCode (Rasterizer*)> draw_fn;
  uint32_t draw_idx;
};

ReturnCode page_export_png(
    const Page& page,
    const DrawCommandList& drawlist,
    std::string* buffer) {
  Rasterizer rasterizer(page.width, page.height, page.dpi);
  rasterizer.clear(page.background_color);

  for (const auto& cmd : drawlist) {
    auto rc = std::visit([&rasterizer] (const auto& c) {
      using T = std::decay_t<decltype(c)>;
      if constexpr (std::is_same_v<T, draw_cmd::Text>)
        return rasterizer.drawText(c.glyphs, c.style, c.transform);
      if constexpr (std::is_same_v<T, draw_cmd::Shape>)
        return rasterizer.drawShape(c.path, c.stroke_style, c.fill_style);

      return ERROR;
    }, cmd);

    if (!rc) {
      return rc;
    }
  }

  *buffer = rasterizer.to_png(); // TODO
  return OK;
}

} // namespace clip

