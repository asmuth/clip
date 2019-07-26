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

namespace fviz {

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

ReturnCode font_get_glyph_path(
    FontRef font,
    double font_size,
    double dpi,
    uint32_t codepoint,
    Path* path) {
  /* load the glyph using freetype */
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

  /* retrieve the glyph outline data from freetype */
  std::vector<Point> glyph_points;
  for (size_t n = 0; n < glyph_outline->n_contours; n++) {
    auto glyph_outline_idx = n == 0 ? 0 : glyph_outline->contours[n - 1] + 1;
    auto glyph_outline_end = glyph_outline->contours[n];

    for (size_t i = glyph_outline_idx; i <= glyph_outline_end; ++i) {
      Point p;
      p.x = glyph_outline->points[i].x / 64;
      p.y = -glyph_outline->points[i].y / 64;

      glyph_points.push_back(p);
    }
  }

  /* convert the glyph outline to a path object */
  *path = Path{};

  for (size_t i = 0; i < glyph_points.size(); ++i) {
    if (i == 0) {
      path->moveTo(glyph_points[i].x, glyph_points[i].y);
    } else {
      path->lineTo(glyph_points[i].x, glyph_points[i].y);
    }
  }

  return OK;
}

bool font_find_fc(
    const std::string& font_pattern,
    std::string* font_file) {
  std::string file;

  {
    auto fc_config = FcInitLoadConfigAndFonts();
    auto fc_pattern = FcNameParse((FcChar8*) font_pattern.c_str());
    FcDefaultSubstitute(fc_pattern);
    FcConfigSubstitute(fc_config, fc_pattern, FcMatchPattern);
    FcResult fc_res;
    auto fc_font = FcFontMatch(fc_config, fc_pattern, &fc_res);
    if (fc_font && fc_res == FcResultMatch) {
      char* fc_file;
      if (FcPatternGetString(fc_font, FC_FILE, 0, (FcChar8**) &fc_file) == FcResultMatch) {
        file = std::string(fc_file);
      }
      FcPatternDestroy(fc_font);
    }
    FcPatternDestroy(fc_pattern);
    FcConfigDestroy(fc_config);
  }

  if (file.empty()) {
    return false;
  }

  *font_file = file;
  return true;
}

ReturnCode font_find(DefaultFont font_name, FontInfo* font_info) {
  std::string font_fc;

  switch (font_name) {
    default:
    case ROMAN_SANS_REGULAR:
      font_fc = "Arial,Helvetica,Helvetica Neue:style=Regular,Roman";
      font_info->font_family_css = "Arial,Helvetica,'Helvetica Neue',sans-serif";
      font_info->font_weight_css = 400;
      break;

    case ROMAN_SANS_MEDIUM:
      font_fc = "Arial,Helvetica,Helvetica Neue:style=Medium,Roman";
      font_info->font_family_css = "Arial,Helvetica,'Helvetica Neue',sans-serif";
      font_info->font_weight_css = 500;
      break;

    case ROMAN_SANS_BOLD:
      font_fc = "Arial,Helvetica,Helvetica Neue:style=Bold,Roman";
      font_info->font_family_css = "Arial,Helvetica,'Helvetica Neue',sans-serif";
      font_info->font_weight_css = 600;
      break;

    case ROMAN_SERIF_REGULAR:
      font_fc = "Times New Romain,serif:style=Regular,Roman";
      font_info->font_family_css = "'Times New Roman', serif";
      font_info->font_weight_css = 400;
      break;

    case ROMAN_SERIF_BOLD:
      font_fc = "Times New Roman,serif:style=Bold,Roman";
      font_info->font_family_css = "'Times New Roman', serif";
      font_info->font_weight_css = 600;
      break;

    case ROMAN_MONOSPACE_REGULAR:
      font_fc = "Courier,serif:style=Regular,Roman";
      font_info->font_family_css = "'Courier', monospace";
      font_info->font_weight_css = 400;
      break;

    case ROMAN_MONOSPACE_BOLD:
      font_fc = "Courier,serif:style=Bold,Roman";
      font_info->font_family_css = "'Courier', monospace";
      font_info->font_weight_css = 600;
      break;
  }

  if (!font_find_fc(font_fc, &font_info->font_file)) {
    return ERROR;
  }

  if (auto rc = font_load(font_info->font_file, &font_info->font); !rc) {
    return errorf(
        ERROR,
        "unble to load font '{}': {}",
        font_info->font_file,
        rc.message);
  }

  return OK;
}

ReturnCode font_find_expr(const Expr* expr, FontInfo* font_info) {
  if (expr_is_value(expr, "roman-sans") ||
      expr_is_value(expr, "roman-sans-regular")) {
    return font_find(ROMAN_SANS_REGULAR, font_info);
  }

  if (expr_is_value(expr, "roman-sans-medium")) {
    return font_find(ROMAN_SANS_MEDIUM, font_info);
  }

  if (expr_is_value(expr, "roman-sans-bold")) {
    return font_find(ROMAN_SANS_BOLD, font_info);
  }

  if (expr_is_value(expr, "roman-serif") ||
      expr_is_value(expr, "roman-serif-regular")) {
    return font_find(ROMAN_SERIF_REGULAR, font_info);
  }
  if (expr_is_value(expr, "roman-serif-bold")) {
    return font_find(ROMAN_SERIF_BOLD, font_info);
  }

  if (expr_is_value(expr, "roman-monospace") ||
      expr_is_value(expr, "roman-monospace-regular")) {
    return font_find(ROMAN_MONOSPACE_REGULAR, font_info);
  }

  if (expr_is_value(expr, "roman-monospace-bold")) {
    return font_find(ROMAN_MONOSPACE_BOLD, font_info);
  }

  std::vector<std::string> font_names;
  std::string font_fc;
  std::string font_style_fc;
  std::string font_css;
  double font_style_css = 400;
  for (auto arg = expr; arg; arg = expr_next(arg)) {
    if (!expr_is_value(arg)) {
      return errorf(
          ERROR,
          "argument error; expected a value, got: {}",
          expr_inspect(expr));
    }

    auto font_name = expr_get_value(arg);
    if (font_name == "Regular" || font_name == "regular") {
      font_style_css = 400;
      font_style_fc = "Regular";
      continue;
    }

    if (font_name == "Medium" || font_name == "medium" ||
        font_name == "Semibold" || font_name == "semibold" ||
        font_name == "Demibold" || font_name == "demibold") {
      font_style_css = 500;
      font_style_fc = "Medium";
      continue;
    }

    if (font_name == "Bold" || font_name == "bold") {
      font_style_css = 600;
      font_style_fc = "Bold";
      continue;
    }

    if (font_name == "Thin" || font_name == "thin" ||
        font_name == "Light" || font_name == "light") {
      font_style_css = 300;
      font_style_fc = "Light";
      continue;
    }

    font_names.emplace_back(font_name);

    if (!font_fc.empty()) {
      font_fc += ",";
    }

    font_fc += font_name;

    if (!font_css.empty()) {
      font_css += ",";
    }

    font_css += "'" + font_name + "'"; // FIXME
  }

  if (!font_style_fc.empty()) {
    font_fc += ":style=" + font_style_fc;
  }

  if (!font_find_fc(font_fc, &font_info->font_file)) {
    return errorf(ERROR, "unble to find font: {}", expr_inspect(expr));
  }

  if (auto rc = font_load(font_info->font_file, &font_info->font); !rc) {
    return errorf(
        ERROR,
        "unble to load font '{}': {}",
        font_info->font_file,
        rc.message);
  }

  font_info->font_family_css = font_css;
  font_info->font_weight_css = font_style_css;

  return OK;
}

} // namespace fviz

