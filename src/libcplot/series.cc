/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "series.h"

namespace stx {
namespace chart {

template <>
Series::Coord<double>::Coord(std::nullptr_t) : value_(0.0) {}

template <>
Series::Coord<int>::Coord(std::nullptr_t) : value_(0) {}

template <>
Series::Coord<std::string>::Coord(std::nullptr_t) : value_("") {}

}
}
