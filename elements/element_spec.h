/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <memory>
#include <string>

namespace signaltk {

struct PropertyList {
  std::list<std::pair<std::string, std::string>> properties;
  std::list<std::pair<std::string, std::unique_ptr<PropertyList>>> children;
};


} // namespace signaltk

