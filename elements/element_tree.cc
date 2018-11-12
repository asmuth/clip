/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "core/plist/plist_parser.h"
#include "element_factory.h"
#include "element_tree.h"
#include "graphics/layer.h"

namespace signaltk {

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


} // namespace signaltk

