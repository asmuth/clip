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
#include "elements/plot/plot_element.h"
#include <unordered_map>

namespace signaltk {

using ElementConfigureFn = std::function<ReturnCode (const PropertyList&, ElementRef*)>;

static std::unordered_map<std::string, ElementConfigureFn> elems = {
  {"plot", &PlotElement::configure}
};

ReturnCode buildElement(
    const std::string& name,
    const PropertyList& plist,
    std::unique_ptr<Element>* elem) {
  const auto& elem_entry = elems.find(name);
  if (elem_entry == elems.end()) {
    return ReturnCode::errorf("NOTFOUND", "no such element: $0", name);
  }

  return elem_entry->second(plist, elem);
}

} // namespace signaltk

