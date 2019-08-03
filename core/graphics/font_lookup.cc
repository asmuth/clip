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
#include "font_lookup.h"
#include "sexpr.h"
#include "graphics/geometry.h"
#include "utils/fileutil.h"

#include <iostream>
#include <functional>

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

using namespace std::placeholders;
using std::bind;

namespace fviz {

const std::string_view DEFAULT_FONT_PATTERN_CSS =
    "Arial,Helvetica,'Helvetica Neue',sans-serif";

const char* const DEFAULT_FONT_PATTERN_FC =
    "Arial,Helvetica,Helvetica Neue:style=Regular,Roman";

struct FontStorage {
  FontStorage() : ft(nullptr), ft_font(nullptr) {}
  FT_Library ft;
  FT_Face ft_font;
};

void font_close(FontStorage* font) {
  if (font->ft_font) {
    FT_Done_Face(font->ft_font);
  }

  if (font->ft) {
    FT_Done_FreeType(font->ft);
  }

  delete font;
}

void* font_get_freetype(FontRef font) {
  return font->ft_font;
}

ReturnCode font_get_glyph_path(
    FontRef font,
    double font_size,
    double dpi,
    uint32_t codepoint,
    Path* path) {
  // load the glyph using freetype
  auto font_size_ft = font_size * (72.0 / dpi) * 64;
  if (FT_Set_Char_Size(font->ft_font, 0, font_size_ft, dpi, dpi)) {
    return ERROR;
  }

  if (FT_Load_Glyph(font->ft_font, codepoint, FT_LOAD_DEFAULT)) {
    return ERROR;
  }

  FT_Glyph glyph;
  if (FT_Get_Glyph(font->ft_font->glyph, &glyph)) {
    return ERROR;
  }

  if (glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
    return ERROR;
  }

  auto glyph_outline = &((FT_OutlineGlyph) glyph)->outline;

  // retrieve the glyph outline data from freetype
  enum class GlyphPointType { MOVE, SIMPLE, CONTROL };

  std::vector<Point> glyph_points;
  std::vector<GlyphPointType> glyph_point_tags;

  auto glyph_outline_tags = glyph_outline->tags;
  for (size_t n = 0; n < glyph_outline->n_contours; n++) {
    auto glyph_outline_idx = n == 0 ? 0 : glyph_outline->contours[n - 1] + 1;
    auto glyph_outline_end = glyph_outline->contours[n];

    // read contour points
    for (size_t i = glyph_outline_idx; i <= glyph_outline_end; ++i) {
      Point p;
      p.x = glyph_outline->points[i].x / 64.0;
      p.y = -glyph_outline->points[i].y / 64.0;

      GlyphPointType pt;
      if (i == glyph_outline_idx) {
        pt = GlyphPointType::MOVE;
      } else if (int(glyph_outline->tags[i]) & 1) {
        pt = GlyphPointType::SIMPLE;
      } else {
        pt = GlyphPointType::CONTROL;
      }

      glyph_points.push_back(p);
      glyph_point_tags.push_back(pt);
    }

    // close the path
    {
      Point p;
      p.x = glyph_outline->points[glyph_outline_idx].x / 64.0;
      p.y = -glyph_outline->points[glyph_outline_idx].y / 64.0;
      glyph_points.push_back(p);
      glyph_point_tags.push_back(GlyphPointType::SIMPLE);
    }
  }

  FT_Done_Glyph(glyph);

  // convert the glyph outline to a path object
  *path = Path{};

  for (size_t i = 0; i < glyph_points.size(); ++i) {
    const auto& p = glyph_points[i];
    const auto& pt = glyph_point_tags[i];

    // move at begin of contour
    if (pt == GlyphPointType::MOVE) {
      path->moveTo(glyph_points[i].x, glyph_points[i].y);
      continue;
    }

    // third order bezier
    if (i + 2 < glyph_points.size() &&
        glyph_point_tags[i + 0] == GlyphPointType::CONTROL &&
        glyph_point_tags[i + 1] == GlyphPointType::CONTROL) {
      //assert(glyph_point_tags[i + 2] != GlyphPointType::CONTROL);

      path->cubicCurveTo(
          glyph_points[i + 0].x,
          glyph_points[i + 0].y,
          glyph_points[i + 1].x,
          glyph_points[i + 1].y,
          glyph_points[i + 2].x,
          glyph_points[i + 2].y);

      i += 2;
      continue;
    }

    // second order bezier
    if (i + 1 < glyph_points.size() &&
        glyph_point_tags[i] == GlyphPointType::CONTROL) {
      //assert(glyph_point_tags[i + 1] != GlyphPointType::CONTROL);

      path->quadraticCurveTo(
          glyph_points[i + 0].x,
          glyph_points[i + 0].y,
          glyph_points[i + 1].x,
          glyph_points[i + 1].y);

      i += 1;
      continue;
    }

    // simple line segments
    path->lineTo(glyph_points[i].x, glyph_points[i].y);
  }

  return OK;
}

ReturnCode font_load(const std::string& font_file, FontRef* font_ref) {
  auto font = FontRef(new FontStorage, bind(&font_close, _1));

  if (FT_Init_FreeType(&font->ft) != 0) {
    return ERROR;
  }

  if (FT_New_Face(font->ft, font_file.c_str(), 0, &font->ft_font) != 0) {
    return ERROR;
  }

  *font_ref = std::move(font);
  return OK;
}

ReturnCode font_load_best(
    const std::string& font_pattern,
    FontInfo* font_info) {
  std::string font_file;

  if (font_pattern.empty()) {
    return error(ERROR, "unable to load font: empty font pattern");
  }

  if (font_pattern[0] == '/' ||
      StringUtil::endsWith(font_pattern, ".otf") ||
      StringUtil::endsWith(font_pattern, ".ttf")) { // TODO improved filename detection
    font_file = font_pattern;
  }

  // lookup using fontconfig
  if (font_file.empty()) {
    auto fc_config = FcInitLoadConfigAndFonts();
    auto fc_pattern = FcNameParse((FcChar8*) font_pattern.c_str());

    FcDefaultSubstitute(fc_pattern);
    FcConfigSubstitute(fc_config, fc_pattern, FcMatchPattern);

    FcResult fc_res;
    auto fc_font = FcFontMatch(fc_config, fc_pattern, &fc_res);
    if (fc_font && fc_res == FcResultMatch) {
      char* fc_file;
      if (FcPatternGetString(fc_font, FC_FILE, 0, (FcChar8**) &fc_file) == FcResultMatch) {
        font_file = std::string(fc_file);
      }

      FcPatternDestroy(fc_font);
    }

    FcPatternDestroy(fc_pattern);
    FcConfigDestroy(fc_config);
  }

  if (font_file.empty()) {
    return errorf(ERROR, "unable to find font: ${}", font_pattern);
  }

  FontRef font_ref;
  if (auto rc = font_load(font_file, &font_ref); !rc) {
    return errorf(
        ERROR,
        "unble to load font '{}': {}",
        font_file,
        rc.message);
  }

  font_info->fonts.insert(font_info->fonts.begin(), font_ref);
  font_info->font_family_css.clear();
  return OK;
}

ReturnCode font_load_defaults(FontInfo* font_info) {
  std::vector<std::string> font_files;

  auto fc_config = FcInitLoadConfigAndFonts();
  auto fc_objs = FcObjectSetBuild(FC_FILE, nullptr);
  auto fc_pattern = FcNameParse((FcChar8*) DEFAULT_FONT_PATTERN_FC);

  FcDefaultSubstitute(fc_pattern);
  FcConfigSubstitute(fc_config, fc_pattern, FcMatchPattern);

  FcResult fc_res;
  FcFontSet* fc_fontset = FcFontSort(0, fc_pattern, FcTrue, 0, &fc_res);

  if (fc_fontset) {
    for (size_t i=0; i < fc_fontset->nfont; ++i) {
      char* fc_file = nullptr;
      auto fc_file_rc = FcPatternGetString(
          fc_fontset->fonts[i],
          FC_FILE,
          0,
          reinterpret_cast<FcChar8**>(&fc_file));

      if (fc_file_rc != FcResultMatch) {
        continue;
      }

      font_files.emplace_back(fc_file);
    }

    FcFontSetDestroy(fc_fontset);
  }

  FcPatternDestroy(fc_pattern);
  FcConfigDestroy(fc_config);

  for (const auto& font_file : font_files) {
    FontRef font_ref;
    if (auto rc = font_load(font_file, &font_ref); !rc) {
      return errorf(
          ERROR,
          "unble to load font '{}': {}",
          font_file,
          rc.message);
    }

    font_info->fonts.push_back(font_ref);
  }

  font_info->font_family_css = DEFAULT_FONT_PATTERN_CSS;
  font_info->font_weight_css = 500;
  return OK;
}

} // namespace fviz

