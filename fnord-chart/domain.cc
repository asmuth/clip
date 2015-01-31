/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-chart/domain.h"
#include "fnord-chart/continuousdomain.h"
#include "fnord-chart/discretedomain.h"
#include "fnord-chart/timedomain.h"

using namespace fnord;
namespace fnord {
namespace chart {

const char AnyDomain::kDimensionLetters[] = "xyz";
const int AnyDomain::kDefaultNumTicks = 8;
const double AnyDomain::kDefaultDomainPadding = 0.1;

template <> Domain<int64_t>*
    Domain<int64_t>::mkDomain() {
  return new ContinuousDomain<int64_t>();
}

template <> Domain<double>*
    Domain<double>::mkDomain() {
  return new ContinuousDomain<double>();
}

template <> Domain<fnord::DateTime>*
    Domain<fnord::DateTime>::mkDomain() {
  return new TimeDomain();
}

template <> Domain<std::string>* Domain<std::string>::mkDomain() {
  return new DiscreteDomain<std::string>();
}

}
}

