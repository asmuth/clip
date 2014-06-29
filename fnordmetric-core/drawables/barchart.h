/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_BARCHART_H
#define _FNORDMETRIC_BARCHART_H
#include <stdlib.h>
#include <assert.h>
#include "../drawable.h"

namespace fnordmetric {

class BarChart : public Drawable {
public:

  void draw(ChartRenderTarget* target) override;

};

}
#endif
