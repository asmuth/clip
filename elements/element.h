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
#include <atomic>
#include <memory>
#include <string>
#include "core/plist/plist.h"
#include "utils/return_code.h"

namespace signaltk {
class Layer;

using plist::PropertyList;

class Element {
public:

  virtual ~Element() = default;

  virtual ReturnCode renderTo(Layer* frame) const = 0;

};

using ElementRef = std::unique_ptr<Element>;


} // namespace signaltk

