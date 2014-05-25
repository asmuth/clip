/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FILEBACKEND_PAGEMANAGER_H
#define _FNORDMETRIC_FILEBACKEND_PAGEMANAGER_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include "../storagebackend.h"

namespace fnordmetric {
namespace filebackend {

/**
 * This is an internal class. For usage instructions and extended documentation
 * please refer to "storagebackend.h" and "filebackend.h"
 */
class PageManager {
  friend class FileBackendTest;
public:
  struct Page {
    uint64_t offset;
    uint64_t size;
    uint64_t used;
  };

  PageManager(size_t end_pos_, size_t block_size);

  /**
   * Request a page from the page manager
   */
  const Page getPage(size_t min_size) {

  }

  /**
   * Return a page to the pagemanager. Adds this page to the freelist
   */
  void yieldPage(const Page& page) {

  }

protected:
  /**
   * Index of the first unused byte in the file
   */
  size_t end_pos_;
};

}
}
#endif
