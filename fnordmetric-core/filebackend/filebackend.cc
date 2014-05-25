/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "filebackend.h"
#include "cursor.h"
#include "streamref.h"
#include "pagemanager.h"
#include "assert.h"

namespace fnordmetric {
namespace filebackend {

FileBackend::FileBackend() : max_stream_id_(0), IStorageBackend() {}

std::unique_ptr<FileBackend> FileBackend::openFile(const std::string& filename) {
  auto backend = new FileBackend();
  return std::unique_ptr<FileBackend>(backend);
}

std::unique_ptr<IStorageCursor> FileBackend::getCursor(const std::string& key) {
  return std::unique_ptr<Cursor>(new Cursor(getStreamRef(key)));
}

// FIXPAUL locking!
std::shared_ptr<StreamRef> FileBackend::getStreamRef(const std::string& key) {
  auto stream_id = getStreamId(key);
  auto iter = stream_refs_.find(stream_id);

  if (iter == stream_refs_.end()) {
    std::shared_ptr<StreamRef> stream_ref(new StreamRef(stream_id, key));
    stream_refs_.insert(std::make_pair(stream_id, stream_ref));
    return stream_ref;
  } else {
    return iter->second;
  }
}

// FIXPAUL must hold lock to call!
uint64_t FileBackend::getStreamId(const std::string& stream_key) {
  auto iter = stream_ids_.find(stream_key);

  if (iter == stream_ids_.end()) {
    auto stream_id = ++max_stream_id_;
    stream_ids_.insert(std::make_pair(stream_key, stream_id));
    return stream_id;
  } else {
    return iter->second;
  }
}

Cursor::Cursor(std::shared_ptr<StreamRef> stream_ref) :
    stream_ref_(stream_ref) {}

uint64_t Cursor::seekTo(uint64_t position) {

}

uint64_t Cursor::seekToFirst() {}
uint64_t Cursor::seekToLast() {}
void Cursor::getRow(const std::function<void (const uint8_t* data,
    size_t len, uint64_t time)>& func) const {}
bool Cursor::next() {}
uint64_t Cursor::appendRow(const std::vector<uint8_t>& data) {}
std::unique_ptr<IStorageCursor> Cursor::clone() const {}

StreamRef::StreamRef(
    uint64_t stream_id,
    const std::string& stream_key) :
    stream_id_(stream_id),
    stream_key_(stream_key) {}

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

MmapPageManager::MmapPageManager(int fd, PageManager&& page_manager) :
  fd_(fd),
  page_manager_(std::move(page_manager)),
  current_mapping_(nullptr) {}


MmapPageManager::~MmapPageManager() {
  if (current_mapping_ != nullptr) {
    current_mapping_->decrRefs();
  }
}

MmapPageManager* MmapPageManager::openFile(int fd) {
  struct stat fd_stat;

  assert(fd > 0);
  if (fstat(fd, &fd_stat) < 0) {
    perror("fstat() failed");
    return nullptr;
  }

  off_t fd_len = lseek(fd, 0, SEEK_END);
  if (fd_len < 0) {
    perror("lseek() failed");
    return nullptr;
  }

  PageManager page_manager(fd_len, fd_stat.st_blksize);

  return new MmapPageManager(fd, std::move(page_manager));
}

MmapPageManager::MmappedPageRef MmapPageManager::allocPage(size_t min_size) {
  auto page = page_manager_.allocPage( min_size);
  uint64_t last_byte = page.offset + page.size;

  ftruncate(fd_, last_byte); // FIXPAUL truncate in chunks + error checking
  return MmappedPageRef(page, getMmapedFile(last_byte));
}

MmapPageManager::MmappedFile* MmapPageManager::getMmapedFile(uint64_t last_byte) {
  // FIXPAUL: get mutex

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
    free(this);
  }
}

MmapPageManager::MmappedPageRef::MmappedPageRef(
    const PageManager::Page& __page,
    MmappedFile* __file) :
    page(__page),
    file(__file) {
  file->incrRefs();
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
