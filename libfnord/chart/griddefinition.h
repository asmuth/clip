/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UI_GRIDDEFINITION_H
#define _FNORDMETRIC_UI_GRIDDEFINITION_H
#include "fnord/chart/domain.h"
#include "fnord/chart/domainprovider.h"

namespace fnord {
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
