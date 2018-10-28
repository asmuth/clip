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
#include <signaltk.h>

namespace signaltk {

int cmd_plot_axes(Context* ctx, const char** args, int arg_count);
int cmd_plot_gridlines(Context* ctx, const char** args, int arg_count);

} // namespace signaltk

