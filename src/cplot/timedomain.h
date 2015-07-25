/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_TIMEDOMAIN_H
#define _libstx_TIMEDOMAIN_H
#include "stx/UnixTime.h"
#include "cplot/continuousdomain.h"

namespace stx {
namespace chart {

class TimeDomain : public ContinuousDomain<stx::UnixTime> {
public:

  TimeDomain(
    stx::UnixTime min_value =
        std::numeric_limits<stx::UnixTime>::max(),
    stx::UnixTime max_value =
        std::numeric_limits<stx::UnixTime>::min(),
    bool is_logarithmic = false,
    bool is_inverted = false);

  std::string label(stx::UnixTime value) const;

};

}
}
#endif
