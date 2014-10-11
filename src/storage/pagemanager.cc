/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
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

namespace fnordmetric {
namespace storage {

PageManager::PageManager(size_t block_size) :
  end_pos_(0),
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

MmapPageManager::MmapPageManager(int fd, size_t len, size_t block_size) :
    PageManager(block_size),
    fd_(fd),
    file_size_(len),
    current_mapping_(nullptr) {}

//MmapPageManager::MmapPageManager(
//    int fd,
//    size_t len,
//    size_t block_size,
//    const LogSnapshot& log_snapshot) :
//    PageManager(block_size, log_snapshot),
//    fd_(fd),
//    file_size_(len),
//    current_mapping_(nullptr) {}

MmapPageManager::MmapPageManager(MmapPageManager&& move) :
    PageManager(std::move(move)),
    fd_(move.fd_),
    file_size_(move.file_size_),
    current_mapping_(move.current_mapping_) {
  move.fd_ = -1;
  move.file_size_ = 0;
  move.current_mapping_ = nullptr;
}

MmapPageManager::~MmapPageManager() {
  if (current_mapping_ != nullptr) {
    current_mapping_->decrRefs();
  }

  close(fd_);
}

void MmapPageManager::fsync() const {
  if (current_mapping_ != nullptr) {
    msync(current_mapping_, file_size_, MS_SYNC);
  }
}

std::unique_ptr<PageManager::PageRef> MmapPageManager::getPage(
    const PageManager::Page& page) {
  uint64_t last_byte = page.offset + page.size;

  mmap_mutex_.lock();

  if (last_byte > file_size_) {
    ftruncate(fd_, last_byte); // FIXPAUL check errors
    file_size_ = last_byte;
  }

  auto page_ref = new MmappedPageRef(page, getMmappedFile(last_byte));
  mmap_mutex_.unlock();

  return std::unique_ptr<PageManager::PageRef>(page_ref);
}

MmapPageManager::MmappedFile* MmapPageManager::getMmappedFile(
    uint64_t last_byte) {
  if (current_mapping_ == nullptr || last_byte > current_mapping_->size) {
    /* align mmap size to the next larger block boundary */
    uint64_t mmap_size =
        ((last_byte + kMmapSizeMultiplier - 1) / kMmapSizeMultiplier) *
        kMmapSizeMultiplier;

    int fd = dup(fd_);
    if (fd < 0) {
      perror("dup() failed");
      abort(); // FIXPAUL
    }

    void* addr = mmap(
        nullptr,
        mmap_size,
        PROT_WRITE | PROT_READ,
        MAP_SHARED,
        fd,
        0);

    if (addr == MAP_FAILED) {
      perror("mmap() failed");
      abort(); // FIXPAUL
    }

    if (current_mapping_ != nullptr) {
      current_mapping_->decrRefs();
    }

    current_mapping_ = new MmappedFile(addr, mmap_size, fd);
  }

  return current_mapping_;
}

MmapPageManager::MmappedFile::MmappedFile(
  void* __data,
  const size_t __size,
  const int __fd) :
  data(__data),
  size(__size),
  fd(__fd),
  refs(1) {}

void MmapPageManager::MmappedFile::incrRefs() {
  refs++;
}

void MmapPageManager::MmappedFile::decrRefs() {
  if (refs.fetch_sub(1) == 1) {
    assert(refs.load() == 0);
    munmap(data, size);
    close(fd);
    delete this;
  }
}

MmapPageManager::MmappedPageRef::MmappedPageRef(
    const PageManager::Page& page,
    MmappedFile* file) :
    PageRef(page),
    file_(file) {

  file_->incrRefs();
}

MmapPageManager::MmappedPageRef::MmappedPageRef(
    MmapPageManager::MmappedPageRef&& move) :
    PageRef(move.page_),
    file_(move.file_) {
  move.file_ = nullptr;
}

void* MmapPageManager::MmappedPageRef::getPtr() const {
  return file_->data;
}

MmapPageManager::MmappedPageRef::~MmappedPageRef() {
  file_->decrRefs();
}

}
}
