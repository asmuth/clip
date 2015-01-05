/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_DOMAINPROVIDER_IMPL_H
#define _FNORDMETRIC_DOMAINPROVIDER_IMPL_H

namespace fnord {
namespace chart {

template <typename T>
T* DomainProvider::getAs() const {
  T* domain = dynamic_cast<T*>(domain_);

  if (domain == nullptr) {
    RAISE(kRuntimeError, "can't convert domain to requested type");
  }

  return domain;
}

}
}
#endif
