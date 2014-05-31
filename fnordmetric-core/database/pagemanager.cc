/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include "pagemanager.h"

namespace fnordmetric {
namespace database {

PageManager::PageManager(size_t end_pos, size_t block_size) :
  end_pos_(end_pos),
  block_size_(block_size) {}

PageManager::PageManager(const PageManager&& move) :
  end_pos_(move.end_pos_),
  block_size_(move.block_size_),
  freelist_(std::move(move.freelist_)) {}

// FIXPAUL hold lock!
PageManager::Page PageManager::allocPage(size_t min_size) {
  PageManager::Page page;

  /* align the request size to the next largest block boundary */
  uint64_t min_size_aligned =
      ((min_size + block_size_ - 1) / block_size_) * block_size_;

  if (!findFreePage(min_size_aligned, &page)) {
    page.offset = end_pos_;
    page.size   = min_size_aligned;
    //page.used   = 0;
    end_pos_   += page.size;
  }

  return page;
}

// FIXPAUL: proper freelist implementation
// FIXPAUL hold lock!
void PageManager::freePage(const PageManager::Page& page) {
  freelist_.emplace_back(std::make_pair(page.size, page.offset));
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

MmapPageManager::MmapPageManager(int fd, size_t len) :
    fd_(fd),
    file_size_(len),
    current_mapping_(nullptr) {}

MmapPageManager::MmapPageManager(MmapPageManager&& move) :
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

// FIXPAUL hold lock!
MmapPageManager::MmappedPageRef MmapPageManager::getPage(
    const PageManager::Page& page) {
  uint64_t last_byte = page.offset + page.size;
  // FIXPAUL: get mutex

  if (last_byte > file_size_) {
    ftruncate(fd_, last_byte); // FIXPAUL truncate in chunks + error checking
    file_size_ = last_byte;
  }

  return MmappedPageRef(page, getMmapedFile(last_byte));
}

MmapPageManager::MmappedFile* MmapPageManager::getMmapedFile(uint64_t last_byte) {
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

// FIXPAUL: locking!
void MmapPageManager::MmappedFile::incrRefs() {
  ++refs;
}

// FIXPAUL: locking!
void MmapPageManager::MmappedFile::decrRefs() {
  if (--refs == 0) {
    munmap(data, size);
    close(fd);
    delete this;
  }
}

MmapPageManager::MmappedPageRef::MmappedPageRef(
    const PageManager::Page& __page,
    MmappedFile* __file) :
    page(__page),
    file(__file) {
  file->incrRefs();
}

MmapPageManager::MmappedPageRef::MmappedPageRef(
    MmapPageManager::MmappedPageRef&& move) :
    page(move.page),
    file(move.file) {
  move.file = nullptr;
}

MmapPageManager::MmappedPageRef::~MmappedPageRef() {
  file->decrRefs();
}

void* MmapPageManager::MmappedPageRef::operator->() const {
  return file->data;
}

void* MmapPageManager::MmappedPageRef::operator*() const {
  return file->data;
}

}
}
