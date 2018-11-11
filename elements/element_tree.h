/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include "utils/return_code.h"
#include "element.h"
#include "element_spec.h"

namespace signaltk {
class Layer;

struct ElementTree {
  std::vector<ElementRef> roots;
};

ReturnCode buildElementTree(
    const PropertyList& plist,
    ElementTree* tree);

ReturnCode buildElementTree(
    const std::string& spec,
    ElementTree* tree);

ReturnCode renderElements(
    const ElementTree& tree,
    Layer* frame);

} // namespace signaltk

