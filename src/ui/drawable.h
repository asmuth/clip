/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_DRAWABLE_H
#define _FNORDMETRIC_DRAWABLE_H
#include <tuple>

namespace fnordmetric {
namespace ui {
class RenderTarget;

class Drawable {
  friend class Canvas;
public:

  virtual ~Drawable() {}

protected:

  virtual void render(
      RenderTarget* target,
      int width,
      int height,
      std::tuple<int, int, int, int>* padding) const = 0;
};

}
}
#endif
