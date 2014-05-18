/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_STORAGEBACKEND_H
#define _FNORDMETRIC_STORAGEBACKEND_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "metric.h"

namespace fnordmetric {

class IStorageCursor {

};

class IStorageBackend {
public:

  IStorageBackend() {}
  IStorageBackend(const IStorageBackend& copy) = delete;
  virtual ~IStorageBackend() {}

  virtual IStorageCursor getCursor(IMetric metric) = 0;

};

}

#endif
