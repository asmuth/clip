/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "signaltk.h"
#include <graphics/image_api.h>
#include <components/plot/plot_api.h>
#include <iostream>

namespace signaltk {

int cmd(
    Context* ctx,
    const char** args,
    int arg_count) {
  for (const auto& cmd : command_list) {
    if (cmd.name.size() > arg_count) {
      continue;
    }

    bool match = true;
    for (size_t i = 0; i < cmd.name.size(); ++i) {
      if (cmd.name[i] != args[i]) {
        match = false;
        break;
      }
    }

    if (!match) {
      continue;
    }

    return cmd.fn(
        ctx,
        args + cmd.name.size(),
        arg_count - cmd.name.size());
  }

  return -1;
}

} // namespace signaltk

