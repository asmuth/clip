# * This file is part of the "plotfx" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# libstx is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.

add_executable(chart_test chart_test.cc)
target_link_libraries(chart_test signaltk-plot signaltk-graphics signaltk-util ${CAIRO_LIBRARIES})
