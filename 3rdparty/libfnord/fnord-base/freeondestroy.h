/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_UTIL_FREEONDESTROY_H
#define _FNORD_UTIL_FREEONDESTROY_H
#include <stdlib.h>

namespace fnord {
namespace util {

class FreeOnDestroy {
public:

  FreeOnDestroy(void* ptr) : ptr_(ptr) {}
  FreeOnDestroy(const FreeOnDestroy& other) = delete;
  FreeOnDestroy& operator=(const FreeOnDestroy& other) = delete;
  ~FreeOnDestroy() { free(ptr_); }

protected:
  void* ptr_;
};

}
}

#endif
