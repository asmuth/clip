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
#include "core/plist/plist_parser.h"
#include "element_factory.h"
#include "element_tree.h"
#include "graphics/layer.h"

namespace plotfx {

ReturnCode buildElementTree(
    const PropertyList& plist,
    ElementTree* tree) {
  for (size_t i = 0; i < plist.size(); ++i) {
    const auto& elem_name = plist[i].name;
    const auto& elem_config = plist[i].child.get();

    std::unique_ptr<Element> elem;
    if (auto rc = buildElement(elem_name, *elem_config, &elem); !rc.isSuccess()) {
      return rc;
    }

    tree->roots.emplace_back(std::move(elem));
  }

  return ReturnCode::success();
}

ReturnCode buildElementTree(
    const std::string& spec,
    ElementTree* tree) {
  PropertyList plist;
  plist::PropertyListParser plist_parser(spec.data(), spec.size());
  if (!plist_parser.parse(&plist)) {
    return ReturnCode::errorf(
        "EPARSE",
        "invalid element specification: $0",
        plist_parser.get_error());
  }

  return buildElementTree(plist, tree);
}

ReturnCode renderElements(
    const ElementTree& tree,
    Layer* frame) {
  for (const auto& e : tree.roots) {
    if (auto rc = e->renderTo(frame); !rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}


} // namespace plotfx

