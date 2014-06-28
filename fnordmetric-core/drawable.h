/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_DRAWABLE_H
#define _FNORDMETRIC_DRAWABLE_H
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include "seriesdefinition.h"

namespace fnordmetric {

class Drawable {
public:

  void addSeries(SeriesDefinition* series) {
    series_.push_back(series);
  }

protected:
  std::vector<SeriesDefinition*> series_;
};

}
#endif
