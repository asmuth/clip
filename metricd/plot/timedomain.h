/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_TIMEDOMAIN_H
#define _FNORDMETRIC_TIMEDOMAIN_H
#include <metricd/ui/continuousdomain.h>
#include <metricd/util/datetime.h>

namespace fnordmetric {
namespace ui {

class TimeDomain : public ContinuousDomain<fnord::util::DateTime> {
public:

  TimeDomain(
    fnord::util::DateTime min_value =
        std::numeric_limits<fnord::util::DateTime>::max(),
    fnord::util::DateTime max_value =
        std::numeric_limits<fnord::util::DateTime>::min(),
    bool is_logarithmic = false,
    bool is_inverted = false);

  std::string label(fnord::util::DateTime value) const;

};

}
}
#endif
