/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "axisdefinition.h"
#include "domain.h"

namespace fnordmetric {
namespace ui {

AxisDefinition::AxisDefinition(
    kPosition axis_position) :
    position_(axis_position) {}

AxisDefinition::AxisDefinition(
    kPosition axis_position,
    Domain* domain) :
    position_(axis_position) {
  auto numerical = dynamic_cast<NumericalDomain *>(domain);

  if (numerical == nullptr) {
    int m = domain->getCardinality();

    for (int n = 0; n < m; n++) {
      double tick = domain->offsetAt(n);
      addTick((double) n / m);
      addLabel((double) n / m + (1.0f / (m + 1)) * 0.5, domain->labelAt(n));
    }

    addTick(1.0);
  } else {
    for (int n = 0; n < domain->getCardinality(); n++) {
      double tick = domain->offsetAt(n);
      addTick(tick);
      addLabel(tick, domain->labelAt(n));
    }
  }
}

void AxisDefinition::addTick(double tick_position) {
  ticks_.push_back(tick_position);
}

const std::vector<double>& AxisDefinition::getTicks() const {
  return ticks_;
}

void AxisDefinition::addLabel(
    double label_position,
    const std::string& label_text) {
  labels_.emplace_back(label_position, label_text);
}

void AxisDefinition::removeLabels() {
  labels_.clear();
}

const std::vector<std::pair<double, std::string>>& AxisDefinition::getLabels()
    const {
  return labels_;
}

bool AxisDefinition::hasLabels() const {
  return labels_.size() > 0;
}

AxisDefinition::kPosition AxisDefinition::getPosition() const {
  return position_;
}

void AxisDefinition::setTitle(const std::string& title) {
  title_ = title;
}

const std::string& AxisDefinition::getTitle() {
  return title_;
}

bool AxisDefinition::hasTitle() const {
  return title_.length() > 0;
}

}
}
