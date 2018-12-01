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
#include "graphics/layout.h"
#include "common/config_helpers.h"

namespace plotfx {

Document::Document() :
    width({Unit::PX, 1200}),
    height({Unit::PX, 600}) {}

ReturnCode buildDocument(
    const PropertyList& plist,
    Document* doc) {
  static const ParserDefinitions pdefs = {
    {"width", std::bind(&parseMeasureProp, std::placeholders::_1, &doc->width)},
    {"height", std::bind(&parseMeasureProp, std::placeholders::_1, &doc->height)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  for (size_t i = 0; i < plist.size(); ++i) {
    const auto& elem_name = plist[i].name;

    if (!plist[i].child) {
      continue;
    }

    const auto& elem_config = plist[i].child.get();

    std::unique_ptr<Element> elem;
    if (auto rc = buildElement(elem_name, *elem_config, &elem); !rc.isSuccess()) {
      return rc;
    }

    doc->roots.emplace_back(std::move(elem));
  }

  return ReturnCode::success();
}

ReturnCode buildDocument(
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

  return buildDocument(plist, tree);
}

ReturnCode renderElements(
    const Document& tree,
    Layer* frame) {
  Rectangle clip(0, 0, frame->width, frame->height);

  for (const auto& e : tree.roots) {
    if (auto rc = e->draw(clip, frame); !rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}


} // namespace plotfx

