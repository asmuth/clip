/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/util/format.h"
#include "metricd/util/time.h"

namespace fnordmetric {

ReturnCode parseTimeSpec(const std::string& str, uint64_t* timestamp) {
  if (str == "now") {
    *timestamp = WallClock::unixMicros();
  }

  // FIXME: support ISO8601
  // FIXME: support -NN<unit>

  try {
    *timestamp = std::stoull(str) * kMicrosPerSecond;
    return ReturnCode::success();
  } catch (...) {
    // ignore
  }

  return ReturnCode::errorf("EARG", "invalid time: $0", str);
}

} // namespace fnordmetric

