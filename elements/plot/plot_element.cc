/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "elements/context.h"
#include "elements/plot/plot_element.h"

namespace signaltk {

static constexpr const char* ID = "plot";

ReturnCode PlotElement::configure(
    const PropertyList& plist,
    std::unique_ptr<Element>* elem) {
  elem->reset(new PlotElement());

  return ReturnCode::success();
}

ReturnCode PlotElement::renderTo(Layer* frame) const {
  // render axes
  for (const auto& axis : config.axes) {
    if (auto rc = renderAxis(axis, frame); rc) {
      return rc;
    }
  }

  return ReturnCode::success();
}

} // namespace signaltk

