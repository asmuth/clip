/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-chart/griddefinition.h"

namespace fnord {
namespace chart {

GridDefinition::GridDefinition(
    kPlacement placement) :
    placement_(placement),
    domain_(nullptr) {}

void GridDefinition::setDomain(DomainProvider* domain) {
  domain_ = domain;
}

GridDefinition::kPlacement GridDefinition::placement() const {
  return placement_;
}

const std::vector<double> GridDefinition::ticks() const {
  if (domain_ == nullptr || domain_->empty()) {
    return std::vector<double>();
  } else {
    return domain_->get()->getTicks();
  }
}

}
}
