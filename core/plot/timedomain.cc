/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "core/util/UnixTime.h"
#include "timedomain.h"

namespace signaltk {
namespace chart {

TimeDomain::TimeDomain(
    UnixTime min_value,
    UnixTime max_value,
    bool is_logarithmic,
    bool is_inverted) :
    ContinuousDomain<UnixTime>(
        min_value,
        max_value,
        is_logarithmic,
        is_inverted) {}

std::string TimeDomain::label(UnixTime value) const {
  auto range = ContinuousDomain<UnixTime>::getRange();

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
