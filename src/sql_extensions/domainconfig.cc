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

namespace fnordmetric {
namespace query {

DomainConfig::DomainConfig(
    ui::Drawable* drawable,
    int dimension) {
  switch (dimension) {
    case 0:
      domain_ = drawable->getDomain(ui::AnyDomain::DIM_X);
    case 1:
      domain_ = drawable->getDomain(ui::AnyDomain::DIM_Y);
    case 2:
      domain_ = drawable->getDomain(ui::AnyDomain::DIM_Z);
    default:
      RAISE(util::RuntimeException, "invalid dimension: %i", dimension);
  }
}

void DomainConfig::setMin(const SValue& value) {
}

void DomainConfig::setMax(const SValue& value) {
}

void DomainConfig::setInvert(bool invert) {
}

void DomainConfig::setLogarithmic(bool logarithmic) {
}

}
}
