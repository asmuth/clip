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
#include <stdlib.h>
#include <vector>
#include <string>

namespace signaltk {
struct Context;

enum Status : int {
  OK = 0,
  ERROR,
  ERROR_IO,
  ERROR_NOT_IMPLEMENTED,
  ERROR_INVALID_ARGUMENT
};

struct Command {
  std::vector<std::string> name;
  int (*fn)(Context*, const char**, int);
};

extern const std::vector<Command> command_list;

int cmd(Context*, const char** args, int arg_count);

} // namespace signaltk

