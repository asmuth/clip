/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stack>
#include "element.h"
#include "graphics/layer.h"

namespace signaltk {

class Context {
public:

  template <typename T>
  Status element_config(T** elem);

  template <typename T>
  Status element_config(T const** elem) const;

  template <typename T>
  Status element_add();

  std::unique_ptr<Layer> frame;

protected:
  std::stack<ElementRef> elements;
};

} // namespace signaltk

#include "context_impl.h"
