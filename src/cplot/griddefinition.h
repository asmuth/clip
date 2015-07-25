/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_UI_GRIDDEFINITION_H
#define _libstx_UI_GRIDDEFINITION_H
#include "cplot/domain.h"
#include "cplot/domainprovider.h"

namespace stx {
namespace chart {

class GridDefinition {
public:

  enum kPlacement {
    GRID_HORIZONTAL = 0,
    GRID_VERTICAL = 1
  };

  /**
   * Create a new grid definition
   */
  GridDefinition(kPlacement placement);

  /**
   * Set the domain for this grid
   */
  void setDomain(DomainProvider* domain);

  kPlacement placement() const;
  const std::vector<double> ticks() const;

protected:
  kPlacement placement_;
  DomainProvider* domain_;
};

}
}
#endif
