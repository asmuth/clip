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
#include "page_export_image.h"
#include "rasterize.h"

using std::bind;
using namespace std::placeholders;

namespace fviz {

struct DrawOp {
  std::function<ReturnCode (Rasterizer*)> draw_fn;
  uint32_t draw_idx;
};

ReturnCode page_export_png(
    const Page& page,
    std::string* buffer) {
  // create a list of drawing operations
  std::vector<DrawOp> ops;
  for (const auto& e : page.text_elements) {
    DrawOp op;
    op.draw_fn = bind(&Rasterizer::drawText, _1, e.glyphs, e.style);
    op.draw_idx = e.zindex.value_or(0);
    ops.push_back(op);
  }

  for (const auto& e : page.shape_elements) {
    DrawOp op;
    op.draw_idx = e.zindex.value_or(0);
    op.draw_fn = bind(
        &Rasterizer::drawShape,
        _1,
        e.path,
        e.stroke_style,
        e.fill_color,
        e.clip);

    ops.push_back(op);
  }

  // sort drawing operations
  std::sort(
      ops.begin(),
      ops.end(),
      [] (const auto& a, const auto& b) {
        return a.draw_idx < b.draw_idx;
      });

  // execute drawing operations
  Rasterizer rasterizer(page.width, page.height, page.dpi);
  rasterizer.clear(page.background_color);

  for (const auto& op : ops) {
    if (auto rc = op.draw_fn(&rasterizer); !rc) {
      return rc;
    }
  }

  // return the image
  *buffer = rasterizer.to_png(); // TODO
  return OK;
}

} // namespace fviz

