/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FILEBACKEND_H
#define _FNORDMETRIC_FILEBACKEND_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include "storagebackend.h"

namespace fnordmetric {

class FileBackend : public IStorageBackend {
public:

  FileBackend() : IStorageBackend() {}
  FileBackend(const FileBackend& copy) = delete;

  static std::unique_ptr<FileBackend> openFile(const std::string& filename);

  virtual IStorageCursor getCursor(IMetric metric) override;

};

}

#endif
