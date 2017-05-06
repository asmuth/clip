/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "cplot/domainprovider.h"

namespace stx {
namespace chart {

DomainProvider::DomainProvider(
    AnyDomain* domain /* = nullptr */) :
    domain_(domain),
    free_on_destroy_(false) {};

DomainProvider::~DomainProvider() {
  if (free_on_destroy_) {
    delete domain_;
  }
}

AnyDomain* DomainProvider::get() const {
  return domain_;
}

bool DomainProvider::empty() const {
  return domain_ == nullptr;
}

void DomainProvider::reset(
    AnyDomain* domain,
    bool free_on_destroy /* = false */) {
  if (free_on_destroy_) {
    delete domain_;
  }

  domain_ = domain;
  free_on_destroy_ = free_on_destroy;
}

const std::vector<double> DomainProvider::getTicks() const {
  if (empty()) {
    return std::vector<double>{};
  } else {
    return domain_->getTicks();
  }
}

const std::vector<std::pair<double, std::string>>
    DomainProvider::getLabels() const {
  if (empty()) {
    return std::vector<std::pair<double, std::string>>{};
  } else {
    return domain_->getLabels();
  }
}


}
}
