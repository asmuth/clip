/**
 * This file is part of the "clip" project
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
#pragma once
#include "command.h"
#include "plot/plotgen.h"
#include "draw/rectangle.h"
#include "figure/legend.h"

namespace clip {

const CommandMap COMMANDS = {
  //{"width", CommandFn(&context_configure)},
  //{"height", CommandFn(&context_configure)},
  //{"dpi", CommandFn(&context_configure)},
  //{"draw/rectangle", CommandFn(&draw::rectangle)},
  {"tools/plotgen", CommandFn(&plot_eval)},
};

} // namespace clip


