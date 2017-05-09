# This file is part of the "libstx" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# libstx is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.

# chart
add_library(cplot STATIC
    axisdefinition.cc
    areachart.cc
    barchart.cc
    linechart.cc
    pointchart.cc
    canvas.cc
    domain.cc
    domainprovider.cc
    drawable.cc
    griddefinition.cc
    legenddefinition.cc
    series.cc
    timedomain.cc)

add_executable(test-cplot chart_test.cc)
target_link_libraries(test-cplot cplot stx-base)
