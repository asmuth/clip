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
#include <graphics/text.h>
#include <graphics/text_shaper.h>
#include "graphics/text_layout.h"
#include "graphics/text_support.h"
#include <graphics/page_description.h>
#include <graphics/brush.h>

namespace clip {

TextStyle::TextStyle() :
    direction(TextDirection::LTR) {}

Status drawTextLabel(
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    const TextStyle& style,
    Page* page) {
  text::TextSpan span;
  span.text_direction = style.direction;
  span.text = text;
  span.font = style.font;
  span.font_size = style.font_size;
  span.span_id = 0;
  span.script = style.default_script;
  span.language = style.default_language;

  if (span.script.empty()) {
    span.script = page->text_default_script;
  }

  if (span.language.empty()) {
    span.language = page->text_default_language;
  }

  text::TextLine line;
  line.spans.push_back(span);
  line.base_direction = style.direction;

  if (auto rc = text_reorder_bidi_line(&line); !rc) {
    return ERROR;
  }

  Rectangle bbox;
  std::vector<text::GlyphPlacementGroup> glyphs;
  if (auto rc = text::text_layout_line(line, page->dpi, &glyphs, &bbox); !rc) {
    return rc;
  }

  auto offset = layout_align(bbox, position, align_x, align_y);

  for (auto& gg : glyphs) {
    for (auto& g : gg.glyphs) {
      g.x += offset.x;
      g.y += offset.y;
    }
  }

  PageTextElement op;
  op.text = text;
  op.glyphs = std::move(glyphs);

  if (rotate) {
    op.transform = mul(
        translate2({position.x, position.y}),
        mul(
            rotate2(rotate),
            translate2({-position.x, -position.y})));
  }

  op.style = style;
  op.origin = offset;

  page_add_text(page, op);
  return OK;
}

Status drawTextLabel(
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    const TextStyle& style,
    Page* page) {
  return drawTextLabel(text, position, align_x, align_y, 0, style, page);
}

Status text_measure_label(
    const std::string& text,
    TextDirection text_direction_base,
    const FontInfo& font,
    double font_size,
    double dpi,
    Rectangle* bbox) {
  text::TextSpan span;
  span.text = text;
  span.text_direction = text_direction_base;
  span.font = font;
  span.font_size = font_size;
  span.span_id = 0;

  text::TextLine line;
  line.spans.push_back(span);
  line.base_direction = text_direction_base;

  return text_layout_line(line, dpi, nullptr, bbox);
}

} // namespace clip

