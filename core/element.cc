/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "element.h"
#include "core/layout.h"

namespace fviz {

int32_t Element::z_index() const {
  auto layout = layout_settings();

  switch (layout.position) {
    case Position::RELATIVE:
      return 1;
    case Position::TOP:
      return 0;
  }

  return -1;
}

} // namespace fviz

