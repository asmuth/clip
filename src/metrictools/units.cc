/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iomanip>
#include <sstream>
#include <metrictools/units.h>
#include <metrictools/util/time.h>

namespace fnordmetric {

std::string formatValue(
    const std::string& value,
    const UnitConfig* unit /* = nullptr */,
    uint64_t rate_base /* = 0 */,
    double scale /* = 1.0 */) try {
  auto val = std::stod(value);
  val *= scale;

  const UnitNameConfig* unit_name = nullptr;
  if (unit) {
    for (const auto& e : unit->names) {
      if (!unit_name || std::stod(e.factor) < val) {
        unit_name = &e;
      } else {
        break;
      }
    }
  }

  std::stringstream s;
  if (unit_name) {
    auto factor = std::stod(unit_name->factor);
    s << std::fixed << std::setprecision(1) << (val / factor);
    s << " ";
    s << unit_name->symbol;
  } else {
    s << std::fixed << std::setprecision(2) << val;
  }

  if (rate_base > 0) {
    s << "/";

    if (rate_base == kMicrosPerSecond) {
      s << "s";
    } else if (rate_base < kMicrosPerMinute) {
      s << (rate_base / kMicrosPerSecond);
      s << "s";
    } else if (rate_base == kMicrosPerMinute) {
      s << "min";
    } else if (rate_base < kMicrosPerHour) {
      s << (rate_base / kMicrosPerMinute);
      s << "min";
    } else if (rate_base == kMicrosPerHour) {
      s << "h";
    } else if (rate_base < kMicrosPerDay) {
      s << (rate_base / kMicrosPerHour);
      s << "h";
    } else if (rate_base == kMicrosPerDay) {
      s << "day";
    } else {
      s << (rate_base / kMicrosPerDay);
      s << "day";
    }
  }

  return s.str();
} catch (const std::exception& e) {
  return value;
}

} // namespace fnordmetric

