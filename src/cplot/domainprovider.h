/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_DOMAINPROVIDER_H
#define _libstx_DOMAINPROVIDER_H
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include "cplot/domain.h"
#include "stx/exception.h"

namespace stx {
namespace chart {

class DomainProvider {
public:
  DomainProvider(AnyDomain* domain = nullptr);
  ~DomainProvider();

  AnyDomain* get() const;
  template <typename T> T* getAs() const;
  bool empty() const;
  void reset(AnyDomain* domain, bool free_on_destroy = false);

  const std::vector<double> getTicks() const;
  const std::vector<std::pair<double, std::string>> getLabels() const;

protected:
  AnyDomain* domain_;
  bool free_on_destroy_;
};

}
}

#include "domainprovider_impl.h"
#endif
