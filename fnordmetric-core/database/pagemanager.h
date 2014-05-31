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
#include <assert.h>
#include "../backend.h"

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
    union {
      uint64_t offset;
      void* ptr;
    };
    uint64_t size;
  };

  PageManager(size_t end_pos_, size_t block_size);
  PageManager(const PageManager& copy) = delete;
  PageManager& operator=(const PageManager& copy) = delete;
  PageManager(const PageManager&& move);

  /**
   * Request a new page from the page manager
   */
  Page allocPage(size_t min_size);

  /**
   * Return a page to the pagemanager. Adds this page to the freelist
   */
  void freePage(const Page& page);

protected:

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
protected:
  struct MmappedFile {
    void* data;
    const size_t size;
    const int fd;
    size_t refs;
    MmappedFile(void* __data, const size_t __size, const int __fd);
    void incrRefs();
    void decrRefs();
  };

public:
  /**
   * Size of the initially create mmaping in bytes. All mmapings will be a
   * multiple of this size!
   */
  static const size_t kMmapSizeMultiplier = 1048576; /* 1 MB */

  struct MmappedPageRef {
    const PageManager::Page page;
    MmappedFile* file;
    MmappedPageRef(const PageManager::Page& __page, MmappedFile* __file);
    MmappedPageRef(MmappedPageRef&& move);
    MmappedPageRef(const MmappedPageRef& copy) = delete;
    MmappedPageRef& operator=(const MmappedPageRef& copy) = delete;
    ~MmappedPageRef();
    void* operator->() const;
    void* operator*() const;
    template<typename T> T* structAt(size_t position);
  };

  explicit MmapPageManager(int fd, size_t len);
  MmapPageManager(MmapPageManager&& move);
  MmapPageManager(const MmapPageManager& copy) = delete;
  MmapPageManager& operator=(const MmapPageManager& copy) = delete;
  ~MmapPageManager();

  /**
   * Request a page to be mapped into memory. Returns a smart pointer.
   */
  MmappedPageRef getPage(const PageManager::Page& page);

protected:

  /**
   * Returns a mmap()ed memory region backend by the managed file spans until
   * at least last_byte
   */
  MmappedFile* getMmapedFile(uint64_t last_byte);

  int fd_;
  size_t file_size_;
  MmappedFile* current_mapping_;
};


/**
 * IMPLEMENTATION
 */
template<typename T>
T* MmapPageManager::MmappedPageRef::structAt(size_t position) {
  assert(position < page.size);
  return (T*) (((char *) file->data) + page.offset + position);
}


}
}
#endif
