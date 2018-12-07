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
#include "document.h"
#include "plist/plist_parser.h"
#include "element_factory.h"
#include "graphics/layer.h"
#include "graphics/layer_svg.h"
#include "graphics/layer_pixmap.h"
#include "graphics/layout.h"
#include "graphics/font_lookup.h"
#include "common/config_helpers.h"
#include "utils/fileutil.h"

namespace plotfx {

Document::Document() :
    width(1200),
    height(600),
    background_colour(Colour::fromRGB(1,1,1)),
    text_colour(Colour::fromRGB(.2,.2,.2)),
    border_colour(Colour::fromRGB(.66,.66,.66)),
    dpi(96),
    font_size(from_pt(11, dpi)) {}

ReturnCode document_setup_defaults(Document* doc) {
  if (!font_load(DefaultFont::HELVETICA_REGULAR, &doc->font_sans)) {
    return ReturnCode::error(
        "EARG",
        "unable to find default sans-sans font (Helvetica/Arial)");
  }

  return OK;
}

ReturnCode document_load(
    const PropertyList& plist,
    Document* doc) {
  if (auto rc = document_setup_defaults(doc); !rc.isSuccess()) {
    return rc;
  }

  // IMPORTANT: parse dpi + font size first

  static const ParserDefinitions pdefs = {
    {"width", std::bind(&configure_measure_rel, std::placeholders::_1, doc->dpi, doc->font_size, &doc->width)},
    {"height", std::bind(&configure_measure_rel, std::placeholders::_1, doc->dpi, doc->font_size, &doc->height)},
    {"background-colour", std::bind(&configure_colour, std::placeholders::_1, &doc->background_colour)},
    {
      "foreground-colour",
      configure_multiprop({
          std::bind(&configure_colour, std::placeholders::_1, &doc->text_colour),
          std::bind(&configure_colour, std::placeholders::_1, &doc->border_colour),
      })
    },
    {"text-colour", std::bind(&configure_colour, std::placeholders::_1, &doc->text_colour)},
    {"border-colour", std::bind(&configure_colour, std::placeholders::_1, &doc->border_colour)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  for (size_t i = 0; i < plist.size(); ++i) {
    const auto& elem_name = plist[i].name;

    if (!plist::is_map(plist[i])) {
      continue;
    }

    const auto& elem_config = plist[i].next.get();

    ElementRef elem;
    if (auto rc = buildElement(*doc, elem_name, *elem_config, &elem); !rc.isSuccess()) {
      return rc;
    }

    doc->roots.emplace_back(std::move(elem));
  }

  return ReturnCode::success();
}

ReturnCode document_load(
    const std::string& spec,
    Document* tree) {
  PropertyList plist;
  plist::PropertyListParser plist_parser(spec.data(), spec.size());
  if (!plist_parser.parse(&plist)) {
    return ReturnCode::errorf(
        "EPARSE",
        "invalid element specification: $0",
        plist_parser.get_error());
  }

  return document_load(plist, tree);
}

ReturnCode document_render_to(
    const Document& tree,
    Layer* layer) {
  Rectangle clip(0, 0, layer->width, layer->height);

  for (const auto& e : tree.roots) {
    if (auto rc = e->draw(tree, clip, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  if (auto rc = layer_submit(layer); !rc.isSuccess()) {
    return rc;
  }

  return ReturnCode::success();
}

ReturnCode document_render(
    const Document& doc,
    const std::string& format,
    const std::string& filename) {
  if (format == "svg")
    return document_render_svg(doc, filename);
  if (format == "png")
    return document_render_png(doc, filename);

  return ReturnCode::errorf("EARG", "invalid output format: $0", format);
}

ReturnCode document_render_svg(
    const Document& doc,
    const std::string& filename) {
  LayerRef layer;

  auto rc = layer_bind_svg(
      doc.width,
      doc.height,
      doc.dpi,
      doc.font_size,
      doc.background_colour,
      [filename] (auto svg) {
        FileUtil::write(filename, Buffer(svg.data(), svg.size()));
        return OK;
      },
      &layer);

  if (!rc.isSuccess()) {
    return rc;
  }

  if (auto rc = document_render_to(doc, layer.get()); !rc.isSuccess()) {
    return rc;
  }

  return OK;
}

ReturnCode document_render_png(
    const Document& doc,
    const std::string& filename) {
  LayerRef layer;

  auto rc = layer_bind_png(
      doc.width,
      doc.height,
      doc.dpi,
      doc.font_size,
      doc.background_colour,
      [filename] (auto png) {
        FileUtil::write(filename, Buffer(png.data(), png.size()));
        return OK;
      },
      &layer);

  if (!rc.isSuccess()) {
    return rc;
  }

  if (auto rc = document_render_to(doc, layer.get()); !rc.isSuccess()) {
    return rc;
  }

  return OK;
}

} // namespace plotfx

