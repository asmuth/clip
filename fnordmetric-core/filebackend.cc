/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "filebackend.h"

namespace fnordmetric {

std::unique_ptr<FileBackend> FileBackend::openFile(const std::string& filename) {
  auto backend = new FileBackend();
  return std::unique_ptr<FileBackend>(backend);
}

std::unique_ptr<IStorageCursor> FileBackend::getCursor(const std::string& key) {
  printf("get_storage_cursor key=%s\n", key.c_str());

  return std::unique_ptr<FileBackendCursor>(new FileBackendCursor());
}

}
