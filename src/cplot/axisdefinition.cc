/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "axisdefinition.h"
#include "domain.h"

namespace stx {
namespace chart {

AxisDefinition::AxisDefinition(
    kPosition axis_position) :
    AxisDefinition(axis_position, nullptr) {}

AxisDefinition::AxisDefinition(
    kPosition axis_position,
    DomainProvider* domain) :
    position_(axis_position),
    domain_(domain),
    has_ticks_(false),
    has_labels_(false) {}

void AxisDefinition::addTick(double tick_position) {
  has_ticks_ = true;
  ticks_.push_back(tick_position);
}

const std::vector<double> AxisDefinition::getTicks() const {
  if (has_ticks_ || domain_ == nullptr) {
    return ticks_;
  }

  return domain_->getTicks();
}

void AxisDefinition::addLabel(
    double label_position,
    const std::string& label_text) {
  has_labels_ = true;
  labels_.emplace_back(label_position, label_text);
}

void AxisDefinition::removeLabels() {
  labels_.clear();
}

const std::vector<std::pair<double, std::string>> AxisDefinition::getLabels()
    const {
  if (has_labels_ || domain_ == nullptr) {
    return labels_;
  }

  return domain_->getLabels();
}

bool AxisDefinition::hasLabels() const {
  return has_labels_ || domain_ != nullptr;
}

AxisDefinition::kPosition AxisDefinition::getPosition() const {
  return position_;
}

void AxisDefinition::setLabelPosition(kLabelPosition pos) {
  printf("set label pos: %i", pos);
}

AxisDefinition::kLabelPosition AxisDefinition::getLabelPosition() const {
  return LABELS_INSIDE;
}

void AxisDefinition::setLabelRotation(double deg) {
  printf("axis label rot: %f\n", deg);
}

double AxisDefinition::getLabelRotation() const {
  return 0.0f;
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

void AxisDefinition::setDomain(DomainProvider* domain) {
  domain_ = domain;
}

}
}
