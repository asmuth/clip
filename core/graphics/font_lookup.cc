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
#include <iostream>
#include <fontconfig/fontconfig.h>
#include "font_lookup.h"
#include "sexpr.h"
#include "utils/fileutil.h"

namespace fviz {


bool findFontSystem(
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

ReturnCode font_load(DefaultFont font_name, FontInfo* font_info) {
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
      font_fc = "CMU Serif,serif:style=Regular,Roman";
      font_info->font_family_css = "'CMU Serif', serif";
      font_info->font_weight_css = 400;
      break;

    case ROMAN_SERIF_MEDIUM:
      font_fc = "CMU Serif,serif:style=Regular,Roman";
      font_info->font_family_css = "'CMU Serif', serif";
      font_info->font_weight_css = 500;
      break;

    case ROMAN_SERIF_BOLD:
      font_fc = "CMU Serif,serif:style=Regular,Roman";
      font_info->font_family_css = "'CMU Serif', serif";
      font_info->font_weight_css = 600;
      break;
  }

  if (!findFontSystem(font_fc, &font_info->font_file)) {
    return ERROR;
  }

  return OK;
}

ReturnCode font_configure(const Expr* expr, FontInfo* font_info) {
  if (expr_is_value(expr, "roman-sans") ||
      expr_is_value(expr, "roman-sans-regular")) {
    return font_load(ROMAN_SANS_REGULAR, font_info);
  }

  if (expr_is_value(expr, "roman-sans-medium")) {
    return font_load(ROMAN_SANS_MEDIUM, font_info);
  }

  if (expr_is_value(expr, "roman-sans-bold")) {
    return font_load(ROMAN_SANS_BOLD, font_info);
  }

  if (expr_is_value(expr, "roman-serif") ||
      expr_is_value(expr, "roman-serif-regular")) {
    return font_load(ROMAN_SERIF_REGULAR, font_info);
  }

  if (expr_is_value(expr, "roman-serif-medium")) {
    return font_load(ROMAN_SERIF_MEDIUM, font_info);
  }

  if (expr_is_value(expr, "roman-serif-bold")) {
    return font_load(ROMAN_SERIF_BOLD, font_info);
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

  if (!findFontSystem(font_fc, &font_info->font_file)) {
    return errorf(ERROR, "unble to find font: {}", expr_inspect(expr));
  }

  font_info->font_family_css = font_css;
  font_info->font_weight_css = font_style_css;
  return OK;
}

} // namespace fviz

