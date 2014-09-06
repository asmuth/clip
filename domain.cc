/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/ui/domain.h>

namespace fnordmetric {
namespace ui {

const char AnyDomain::kDimensionLetters[] = "xyz";
const int AnyDomain::kDefaultNumTicks = 8;
const double AnyDomain::kDefaultDomainPadding = 0.1;

template <> Domain<int>* Domain<int>::mkDomain() {
  return new ContinuousDomain<int>();
}

template <> Domain<double>* Domain<double>::mkDomain() {
  return new ContinuousDomain<double>();
}

template <> Domain<std::string>* Domain<std::string>::mkDomain() {
  return new DiscreteDomain<std::string>();
}

}
}

