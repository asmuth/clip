/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <iostream>
#include <fontconfig/fontconfig.h>
#include "font_lookup.h"
#include "utils/fileutil.h"

namespace plotfx {


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

} // namespace plotfx

