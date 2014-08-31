/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "series.h"

namespace fnordmetric {

template <>
Series::Coord<double>::Coord(std::nullptr_t) : value_(0.0) {}

template <>
Series::Coord<int>::Coord(std::nullptr_t) : value_(0) {}

}
