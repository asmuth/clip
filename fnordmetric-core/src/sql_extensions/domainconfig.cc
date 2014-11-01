/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql_extensions/domainconfig.h>
#include <fnordmetric/ui/continuousdomain.h>

namespace fnordmetric {
namespace query {

DomainConfig::DomainConfig(
    ui::Drawable* drawable,
    ui::AnyDomain::kDimension dimension) :
    domain_(drawable->getDomain(dimension)),
    dimension_letter_(ui::AnyDomain::kDimensionLetters[dimension]) {}

void DomainConfig::setMin(const SValue& value) {
  auto int_domain =
      dynamic_cast<ui::ContinuousDomain<fnordmetric::IntegerType>*>(domain_);
  if (int_domain != nullptr) {
    int_domain->setMin(value.getValue<fnordmetric::IntegerType>());
    return;
  }

  auto float_domain =
      dynamic_cast<ui::ContinuousDomain<fnordmetric::FloatType>*>(domain_);
  if (float_domain != nullptr) {
    float_domain->setMin(value.getValue<fnordmetric::FloatType>());
    return;
  }

  RAISE(
      kRuntimeError,
      "TypeError: can't set min value for %c domain",
      dimension_letter_);
}

void DomainConfig::setMax(const SValue& value) {
  auto int_domain =
      dynamic_cast<ui::ContinuousDomain<fnordmetric::IntegerType>*>(domain_);
  if (int_domain != nullptr) {
    int_domain->setMax(value.getValue<fnordmetric::IntegerType>());
    return;
  }

  auto float_domain =
      dynamic_cast<ui::ContinuousDomain<fnordmetric::FloatType>*>(domain_);
  if (float_domain != nullptr) {
    float_domain->setMax(value.getValue<fnordmetric::FloatType>());
    return;
  }

  RAISE(
      kRuntimeError,
      "TypeError: can't set max value for %c domain",
      dimension_letter_);
}

void DomainConfig::setInvert(bool invert) {
  if (!invert) {
    return;
  }

  domain_->setInverted(invert);
}

void DomainConfig::setLogarithmic(bool logarithmic) {
  if (!logarithmic) {
    return;
  }

  auto continuous_domain = dynamic_cast<ui::AnyContinuousDomain*>(domain_);
  if (continuous_domain == nullptr) {
    RAISE(
        kRuntimeError,
        "TypeError: can't set LOGARITHMIC for discrete domain %c",
        dimension_letter_);
  } else {
    continuous_domain->setLogarithmic(logarithmic);
  }
}

}
}
