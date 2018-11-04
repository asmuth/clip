/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <signaltk.h>
#include <signaltk/plot/plot_api.h>

namespace signaltk {

const std::vector<Command> command_list {

  // command: plot axes
  Command {
    .name = {"plot", "axes"},
    .fn = &cmd_plot_axes,
  },

  // command: plot gridlines
  Command {
    .name = {"plot", "gridlines"},
    .fn = &cmd_plot_gridlines,
  },

};

} // namespace signaltk

