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

class FileBackendCursor : public IStorageCursor {
public:

  FileBackendCursor() {}
  virtual uint64_t seekTo(uint64_t position) override {}
  virtual uint64_t seekToFirst() override {}
  virtual uint64_t seekToLast() override {}
  virtual size_t read(
    size_t n,
    std::vector<std::tuple<uint64_t, const std::vector<uint8_t>>>* destination)
    override {}

  virtual uint64_t appendRow(const std::vector<uint8_t>& data) override {
    printf("write row in filebackend: %lu\n", data.size());
  }

};

class FileBackend : public IStorageBackend {
public:

  FileBackend() : IStorageBackend() {}
  FileBackend(const FileBackend& copy) = delete;

  static std::unique_ptr<FileBackend> openFile(const std::string& filename);

  virtual std::unique_ptr<IStorageCursor> getCursor(
      const IMetricKey& metric_key) override;

};

}

#endif
