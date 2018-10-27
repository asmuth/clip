# This file is part of the "signaltk" project
#   Copyright (c) 2018 Paul Asmuth
#
# libstx is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.

add_library(signaltk-util STATIC
    random.cc
    bufferutil.cc
    exception.cc
    UnixTime.cc
    inspect.cc
    stringutil.cc
    duration.cc
    CivilTime.cc
    buffer.cc
    fileutil.cc
    file.cc
    outputstream.cc
    inputstream.cc
    ISO8601.cc)

