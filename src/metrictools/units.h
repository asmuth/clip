/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <metrictools/util/return_code.h>
#include <string>
#include <map>

namespace fnordmetric {

struct UnitNameConfig {
  std::string name;
  std::string factor;
  std::string singular;
  std::string plural;
  std::string symbol;
};

struct UnitConfig {
  std::string unit_id;
  std::string description;
  std::vector<UnitNameConfig> names;
};

std::string formatValue(
    const std::string& value,
    const UnitConfig* unit = nullptr,
    double scale = 1.0);

} // namespace fnordmetric

