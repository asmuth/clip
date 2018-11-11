/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "element_factory.h"
#include "element_tree.h"
#include "element_spec_parser.h"
#include "graphics/layer.h"

namespace signaltk {

ReturnCode buildElementTree(
    const PropertyList& plist,
    ElementTree* tree) {
  for (size_t i = 0; i < plist.children.size(); ++i) {
    const auto& elem_name = plist.children[i].first;
    const auto& elem_config = plist.children[i].second;

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
  SpecParser parser(spec.data(), spec.size());
  if (auto rc = parser.parse(&plist); !rc.isSuccess()) {
    return rc;
  }

  return buildElementTree(plist, tree);
}

} // namespace signaltk

