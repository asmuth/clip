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
#pragma once
#include <memory>
#include "utils/return_code.h"
#include "source/color_scheme.h"
#include "source/data_model.h"
#include "graphics/measure.h"
#include "graphics/color.h"
#include "graphics/text.h"
#include "element.h"

namespace plotfx {
class Layer;

struct Context {
  std::unique_ptr<Document> document;
  mutable std::string error;
};

struct Document {
  Document();
  Measure width;
  Measure height;
  ColorScheme color_scheme;
  Color background_color;
  Color text_color;
  Color border_color;
  FontInfo font_serif;
  FontInfo font_sans;
  FontInfo font_mono;
  ElementRef root;
  DataContext data;
  double dpi;
  Measure font_size;
};

ReturnCode document_load(
    const PropertyList& plist,
    Document* tree);

ReturnCode document_load(
    const std::string& spec,
    Document* tree);

ReturnCode document_render(
    const Document& doc,
    const std::string& format,
    const std::string& filename);

ReturnCode document_render_to(
    const Document& tree,
    Layer* layer);

ReturnCode document_render_svg(
    const Document& doc,
    const std::string& filename);

ReturnCode document_render_png(
    const Document& doc,
    const std::string& filename);

void ctx_seterrf(plotfx_t* ctx, const std::string& err);
void ctx_seterr(plotfx_t* ctx, const ReturnCode& err);

} // namespace plotfx

