/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "stx/UnixTime.h"
#include "cplot/timedomain.h"

namespace stx {
namespace chart {

TimeDomain::TimeDomain(
    stx::UnixTime min_value,
    stx::UnixTime max_value,
    bool is_logarithmic,
    bool is_inverted) :
    ContinuousDomain<stx::UnixTime>(
        min_value,
        max_value,
        is_logarithmic,
        is_inverted) {}

std::string TimeDomain::label(stx::UnixTime value) const {
  auto range = ContinuousDomain<stx::UnixTime>::getRange();

  if (range < 60 * 60) {
    return value.toString("%H:%M:%S");
  } else if (range < 60 * 60 * 24) {
    return value.toString("%H:%M");
  } else {
    return value.toString("%Y-%m-%d %H:%M");
  }
}

}
}
