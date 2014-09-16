/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/ui/timedomain.h>
#include <fnordmetric/util/datetime.h>

namespace fnordmetric {
namespace ui {

TimeDomain::TimeDomain(
    util::DateTime min_value,
    util::DateTime max_value,
    bool is_logarithmic,
    bool is_inverted) :
    ContinuousDomain<util::DateTime>(
        min_value,
        max_value,
        is_logarithmic,
        is_inverted) {}

std::string TimeDomain::label(util::DateTime value) const {
  return fnordmetric::util::format::formatTimeWithRange(
      value,
      static_cast<uint64_t>(ContinuousDomain<util::DateTime>::getRange()));
}

}
}
