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
#include <functional>
#include "context.h"
#include "return_code.h"
#include "sexpr.h"

namespace clip {

using CommandFnRef = ReturnCode (*) (Context*, const Expr*);
using CommandFn = std::function<ReturnCode (Context*, const Expr*)>;

struct Command {
  Command(CommandFn f) : fn(f) {}
  CommandFn fn;
};

using CommandMap = std::unordered_map<std::string, Command>;

CommandFn bind_cmd(CommandFnRef ref);

} // namespace clip


