/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FILEBACKEND_STREAMREF_H
#define _FNORDMETRIC_FILEBACKEND_STREAMREF_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include "pagemanager.h"

namespace fnordmetric {
namespace filebackend {

class FileBackend;
class Cursor;

/**
 * This is an internal class. For usage instructions and extended documentation
 * please refer to "storagebackend.h" and "filebackend.h"
 */
class StreamRef {
public:
  // FIXPAUL reader writer lock on page alloc?
  struct PageAlloc {
    PageManager::Page page;
    size_t used; /* number of used bytes in the page */
    uint64_t t0; /* time of the first row in the page */
    uint64_t t1; /* time of the last row in the page */
  };

  struct __attribute__((__packed__)) RowHeader {
    uint64_t checksum;
    uint64_t time;
    uint64_t size;
    uint8_t data[];
    void computeChecksum();
  };

  explicit StreamRef(
      FileBackend* backed,
      uint64_t stream_id,
      const std::string& stream_key);

  StreamRef(const StreamRef& copy) = delete;
  StreamRef& operator=(const StreamRef& copy) = delete;

  uint64_t appendRow(const std::vector<uint8_t>& data);

protected:
  std::vector<PageAlloc> pages_;
  FileBackend* backend_;
  const uint64_t stream_id_;
  const std::string stream_key_;
};

/**
 * This is an internal class. For usage instructions and extended documentation
 * please refer to "storagebackend.h" and "filebackend.h"
 */
class StreamDescriptor : public IBackend::IStreamDescriptor {
public:
  explicit StreamDescriptor(std::shared_ptr<StreamRef> stream_ref);
  StreamDescriptor(const StreamDescriptor& copy) = delete;
  StreamDescriptor& operator=(const StreamDescriptor& copy) = delete;
  uint64_t appendRow(const std::vector<uint8_t>& data) override;
  std::unique_ptr<IBackend::IStreamCursor> getCursor() override;
protected:
  std::shared_ptr<StreamRef> stream_ref_;
};

}
}
#endif
