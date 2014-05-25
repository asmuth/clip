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
  };

  /**
   * Request a new page from the page manager
   */
  Page allocPage(size_t min_size);

  /**
   * Return a page to the pagemanager. Adds this page to the freelist
   */
  void freePage(const Page& page);

protected:
  PageManager(size_t end_pos_, size_t block_size);

  /**
   * Try to find a free page with a size larger than or equal to min_size
   *
   * Returns true if a matching free page was found and returns the page into
   * the destination parameter. Returns false if no matching page was found and
   * does not change the destination parameter.
   */
  bool findFreePage(size_t min_size, Page* destination);

  /**
   * Index of the first unused byte in the file
   */
  size_t end_pos_;

  /**
   * Optimal block size for the underlying file
   */
  const size_t block_size_;

  /**
   * Page free list
   *
   * tuple is (size, offset)
   */
  std::vector<std::pair<uint64_t, uint64_t>> freelist_;

};

class MmapPageManager {
  friend class FileBackendTest;
public:
  struct MmappedPage {
    const PageManager::Page page;
    const MmapPageManager* manager;
    const uint8_t* data;
    size_t refs;
    MmappedPage(
        const PageManager::Page __page,
        const MmapPageManager* __manager,
        const uint8_t* __data);
    MmappedPage(const MmappedPage& copy) = delete;
    MmappedPage& operator=(const MmappedPage& copy) = delete;
  };

  struct MmappedPageRef {
    MmappedPage* ptr;
    MmappedPageRef(MmappedPage* __ptr);
    ~MmappedPageRef();
    MmappedPageRef(const MmappedPageRef& copy) = delete;
    MmappedPageRef& operator=(const MmappedPageRef& copy) = delete;
  };

  /**
   * Create a new mmap page manager and hand over ownership of the provided
   * filedescriptor.
   */
  MmapPageManager(int fd);

  /**
   * Request a new page and a pointer to the pages data mapped into memory.
   */
  MmappedPageRef allocPage(size_t min_size);

  /**
   * Request a an exisiting page to be mapped into memory. Every requested page
   * must be returned with yieldpage eventually
   */
  MmappedPageRef getPage(size_t min_size);

  /**
   * Return a page to the pagemanager. Adds this page to the freelist
   */
  //void freePage(const Page& page) override;

protected:

  //void unmmapPage();

  std::unordered_map<uint64_t, MmapedPage*> mapped_pages_;
};

}
}
#endif
