/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <graphics/text.h>
#include <graphics/text_shaper.h>

namespace signaltk {
namespace text {

/**
 * Layout a single line of text
 */
Status layoutText(
    const std::string& text,
    double x,
    double y,
    const FontInfo& font_info,
    TextDirection direction,
    TextHAlign halign,
    TextVAlign valign,
    TextShaper* shaper,
    std::function<void (const GlyphPlacement&)> glyph_cb);

} // namespace text
} // namespace signaltk

