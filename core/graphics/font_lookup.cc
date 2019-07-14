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
  std::string font_css;
  std::string font_fc;

  switch (font_name) {
    default:
    case SANS_REGULAR:
    case HELVETICA_REGULAR:
      font_css = "Arial,Helvetica,'Helvetica Neue',sans-serif";
      font_fc = "Arial,Helvetica,Helvetica Neue:style=Regular,Roman";
      break;

    case SANS_MEDIUM:
    case HELVETICA_MEDIUM:
      font_css = "Arial,Helvetica,'Helvetica Neue',sans-serif";
      font_fc = "Arial,Helvetica,Helvetica Neue:style=Medium,Roman";
      break;

    case SANS_BOLD:
    case HELVETICA_BOLD:
      font_css = "Arial,Helvetica,'Helvetica Neue',sans-serif";
      font_fc = "Arial,Helvetica,Helvetica Neue:style=Bold,Roman";
      break;
  }

  font_info->font_family_css = font_css;

  if (!findFontSystem(font_fc, &font_info->font_file)) {
    return ERROR;
  }

  return OK;
}

} // namespace fviz

