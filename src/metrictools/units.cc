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

namespace fnordmetric {

std::string formatValue(
    const std::string& value,
    const UnitConfig* unit /* = nullptr */,
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
    s << std::fixed << std::setprecision(1) << val;
  }

  return s.str();
} catch (const std::exception& e) {
  return value;
}

} // namespace fnordmetric

