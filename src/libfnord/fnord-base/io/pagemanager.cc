/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include "pagemanager.h"
#include <fnord-base/io/file.h>
#include <fnord-base/io/fileutil.h>

namespace fnord {
namespace io {

PageManager::PageManager(
  size_t block_size,
  size_t end_pos /* = 0 */) :
  end_pos_(end_pos),
  block_size_(block_size) {}

//PageManager::PageManager(size_t block_size, const LogSnapshot& log_snapshot) :
//  block_size_(block_size),
//  end_pos_(log_snapshot.last_used_byte) {}

PageManager::PageManager(const PageManager&& move) :
  end_pos_(move.end_pos_),
  block_size_(move.block_size_),
  freelist_(std::move(move.freelist_)) {}

PageManager::Page PageManager::allocPage(size_t min_size) {
  PageManager::Page page;

  /* align the request size to the next largest block boundary */
  uint64_t min_size_aligned =
      ((min_size + block_size_ - 1) / block_size_) * block_size_;

  mutex_.lock();
  if (!findFreePage(min_size_aligned, &page)) {
    page.offset = end_pos_;
    page.size   = min_size_aligned;
    end_pos_   += page.size;
  }
  mutex_.unlock();

  return page;
}

// FIXPAUL: proper freelist implementation
void PageManager::freePage(const PageManager::Page& page) {
  mutex_.lock();
  freelist_.emplace_back(std::make_pair(page.size, page.offset));
  mutex_.unlock();
}

// FIXPAUL: proper freelist implementation
bool PageManager::findFreePage(size_t min_size, Page* destination) {
  for (auto iter = freelist_.begin(); iter != freelist_.end(); ++iter) {
    if (std::get<0>(*iter) >= min_size) {
      destination->offset = std::get<1>(*iter);
      destination->size   = std::get<0>(*iter);
      //destination->used   = 0;
      freelist_.erase(iter);
      return true;
    }
  }

  return false;
}

PageManager::Page::Page(uint64_t offset_, uint64_t size_) :
    offset(offset_),
    size(size_) {}

PageManager::Page::Page() : offset(0), size(0) {}

PageManager::PageRef::PageRef(const PageManager::Page& page) :
    page_(page) {}

void* PageManager::PageRef::operator*() const {
  return getPtr();
}

PageManager::PageRef::~PageRef() {};

MmapPageManager::MmapPageManager(
    const std::string& filename,
    size_t file_size) :
    PageManager(1, file_size),
    filename_(filename),
    file_size_(file_size),
    used_bytes_(file_size),
    current_mapping_(nullptr) {
  sys_page_size_ = sysconf(_SC_PAGESIZE);
}

MmapPageManager::MmapPageManager(MmapPageManager&& move) :
    PageManager(std::move(move)),
    filename_(move.filename_),
    file_size_(move.file_size_),
    used_bytes_(move.used_bytes_),
    current_mapping_(std::move(move.current_mapping_)),
    sys_page_size_(move.sys_page_size_) {
  move.file_size_ = 0;
  move.used_bytes_ = 0;
}

MmapPageManager::~MmapPageManager() {
  //if (current_mapping_ != nullptr) {
  //  current_mapping_->decrRefs();
  //}
}

void MmapPageManager::shrinkFile() {
  FileUtil::truncate(filename_, used_bytes_);
  file_size_ = used_bytes_;
}

std::unique_ptr<PageManager::PageRef> MmapPageManager::getPage(
    const PageManager::Page& page) {
  return getPageImpl(page, true);
}

std::unique_ptr<PageManager::PageRef> MmapPageManager::getPage(
    const PageManager::Page& page,
    kNoPadding no_padding) {
  return getPageImpl(page, false);
}

std::unique_ptr<PageManager::PageRef> MmapPageManager::getPageImpl(
    const PageManager::Page& page,
    bool allow_padding) {
  uint64_t last_byte = page.offset + page.size;

  mmap_mutex_.lock();

  if (last_byte > used_bytes_) {
    used_bytes_ = last_byte;
  }

  if (last_byte > file_size_) {
    size_t new_size;
    if (allow_padding) {
      auto mmap_block_size = sys_page_size_ * kMmapSizeMultiplier;
      new_size = ((last_byte / mmap_block_size) + 1) * mmap_block_size;
    } else {
      new_size = last_byte;
    }

/*
    if (fnordmetric::env()->verbose()) {
      fnordmetric::env()->logger()->printf(
          "DEBUG",
          "Truncating file %s to %lu bytes",
          filename_.c_str(),
          (long unsigned) new_size);
    }
*/

    FileUtil::truncate(filename_, new_size);
    file_size_ = new_size;
  }

  auto page_ref = new MmappedPageRef(
      page,
      getMmappedFile(last_byte),
      sys_page_size_);

  mmap_mutex_.unlock();

  return std::unique_ptr<PageManager::PageRef>(page_ref);
}

RefPtr<MmapPageManager::MmappedFile> MmapPageManager::getMmappedFile(
    uint64_t last_byte) {
  if (current_mapping_.get() == nullptr ||
      last_byte > current_mapping_->size) {
    /* align mmap size to the next larger block boundary */
    auto file = fnord::File::openFile(
        filename_,
        File::O_READ | File::O_WRITE);

    auto mmap_size = file.size();
    void* addr = nullptr;
    if (mmap_size > 0) {
      addr = mmap(
          nullptr,
          mmap_size,
          PROT_WRITE | PROT_READ,
          MAP_SHARED,
          file.fd(),
          0);

      if (addr == MAP_FAILED) {
        RAISE_ERRNO(kMallocError, "mmap() failed");
      }
    }

    current_mapping_ = RefPtr<MmappedFile>(new MmappedFile(addr, mmap_size));
  }

  return current_mapping_;
}

MmapPageManager::MmappedFile::MmappedFile(
  void* __data,
  const size_t __size) :
  data(__data),
  size(__size) {}

MmapPageManager::MmappedFile::~MmappedFile() {
  munmap(data, size);
}

MmapPageManager::MmappedPageRef::MmappedPageRef(
    const PageManager::Page& page,
    RefPtr<MmappedFile> file,
    size_t sys_page_size) :
    PageRef(page),
    file_(file),
    sys_page_size_(sys_page_size) {}

MmapPageManager::MmappedPageRef::MmappedPageRef(
    MmapPageManager::MmappedPageRef&& move) :
    PageRef(move.page_),
    file_(move.file_),
    sys_page_size_(move.sys_page_size_) {
  move.file_ = nullptr;
}

void* MmapPageManager::MmappedPageRef::getPtr() const {
  return file_->data;
}

void MmapPageManager::MmappedPageRef::sync(bool async /* = false */) const {
  uintptr_t ptr = (uintptr_t) ((char *) getPtr()) + page_.offset;
  auto sptr = (ptr / (sys_page_size_)) * sys_page_size_;
  auto ssize = page_.size + (ptr - sptr);
  msync((void *) sptr, ssize, async ? MS_SYNC : MS_ASYNC);
}

MmapPageManager::MmappedPageRef::~MmappedPageRef() {}

}
}
