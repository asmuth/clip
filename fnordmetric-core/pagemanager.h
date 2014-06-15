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
#include <assert.h>
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <atomic>

namespace fnordmetric {

/**
 * This is an internal class. For usage instructions and extended documentation
 * please refer to "storagebackend.h" and "database.h"
 */
class PageManager {
  friend class DatabaseTest;
public:

  struct Page {
    Page(uint64_t offset_, uint64_t size_);
    Page();
    uint64_t offset;
    uint64_t size;
  };

  struct PageRef {
  public:
    PageRef(const Page& page);
    PageRef(const PageRef& copy) = delete;
    PageRef& operator=(const PageRef& copy) = delete;
    virtual ~PageRef();
    void* operator*() const;
    template<typename T> T* structAt(size_t position) const;
    virtual void* getPtr() const = 0;
    const PageManager::Page page_;
  };

  PageManager(size_t block_size);
  //PageManager(size_t block_size, const LogSnapshot& log_snapshot);
  PageManager(const PageManager& copy) = delete;
  PageManager& operator=(const PageManager& copy) = delete;
  PageManager(const PageManager&& move);

  /**
   * Request a new page from the page manager
   */
  virtual Page allocPage(size_t min_size);

  /**
   * Return a page to the pagemanager. Adds this page to the freelist
   */
  virtual void freePage(const Page& page);

  /**
   * Request a page to be mapped into memory. Returns a smart pointer.
   */
  virtual std::unique_ptr<PageRef> getPage(const PageManager::Page& page) = 0;

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

  std::mutex mutex_;
};

class MmapPageManager : public PageManager {
  friend class DatabaseTest;
protected:
  class MmappedFile {
  public:
    MmappedFile(void* __data, const size_t __size, const int __fd);
    MmappedFile(const MmappedFile& copy) = delete;
    MmappedFile& operator=(const MmappedFile& copy) = delete;
    void* const data;
    const size_t size;
    void incrRefs();
    void decrRefs();
  protected:
    const int fd;
    std::atomic_int refs;
  };

public:
  /**
   * Size of the initially create mmaping in bytes. All mmapings will be a
   * multiple of this size!
   */
  static const size_t kMmapSizeMultiplier = 1048576; /* 1 MB */

  class MmappedPageRef : public PageManager::PageRef {
  public:
    MmappedPageRef(const PageManager::Page& page, MmappedFile* file);
    MmappedPageRef(MmappedPageRef&& move);
    MmappedPageRef(const MmappedPageRef& copy) = delete;
    MmappedPageRef& operator=(const MmappedPageRef& copy) = delete;
    ~MmappedPageRef();
  protected:
    void* getPtr() const override;
    MmappedFile* file_;
  };

  /**
   * Create a new mmap page manager for a new file
   */
  explicit MmapPageManager(int fd, size_t len, size_t block_size);

  /**
   * Create a new mmap page manager for a file where some pages are already
   * allocaed
   */
  //explicit MmapPageManager(
  //    int fd,
  //    size_t len,
  //    size_t block_size);
      //const LogSnapshot& log_snapshot);

  MmapPageManager(MmapPageManager&& move);
  MmapPageManager(const MmapPageManager& copy) = delete;
  MmapPageManager& operator=(const MmapPageManager& copy) = delete;
  ~MmapPageManager();

  /**
   * Request a page to be mapped into memory. Returns a smart pointer.
   */
  std::unique_ptr<PageManager::PageRef> getPage(
      const PageManager::Page& page) override;

protected:

  /**
   * Returns a mmap()ed memory region backend by the managed file spans until
   * at least last_byte
   */
  MmappedFile* getMmapedFile(uint64_t last_byte);

  int fd_;
  size_t file_size_;
  MmappedFile* current_mapping_;
  std::mutex mmap_mutex_;
};


/**
 * IMPLEMENTATION
 */
template<typename T>
T* PageManager::PageRef::structAt(size_t position) const {
  assert(position < page_.size);
  return (T*) (((char *) getPtr()) + page_.offset + position);
}


}
#endif
