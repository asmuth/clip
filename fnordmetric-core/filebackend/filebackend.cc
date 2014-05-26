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
#include <sys/fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "filebackend.h"
#include "cursor.h"
#include "streamref.h"
#include "pagemanager.h"

namespace fnordmetric {
namespace filebackend {

FileBackend::FileBackend(
    PageManager&& page_manager,
    MmapPageManager&& mmap_manager) :
    page_manager_(std::move(page_manager)),
    mmap_manager_(std::move(mmap_manager)),
    max_stream_id_(0) {}

std::unique_ptr<FileBackend> FileBackend::openFile(const std::string& filename) {
  FileBackend* ptr = nullptr;

  int fd = open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    perror("open() failed");
    return std::unique_ptr<FileBackend>(nullptr);
  }

  struct stat fd_stat;
  if (fstat(fd, &fd_stat) < 0) {
    perror("fstat() failed");
    return std::unique_ptr<FileBackend>(nullptr);
  }

  off_t fd_len = lseek(fd, 0, SEEK_END);
  if (fd_len < 0) {
    perror("lseek() failed");
    return std::unique_ptr<FileBackend>(nullptr);
  }

  MmapPageManager mmap_manager(fd, fd_len);

  // FIXPAUL last used page should be from log
  size_t last_used_byte = 0;

  auto backend = new FileBackend(
      PageManager(last_used_byte, fd_stat.st_blksize),
      std::move(mmap_manager));

  return std::unique_ptr<FileBackend>(backend);
}

std::unique_ptr<IBackend::IStreamDescriptor> FileBackend::openStream(
    const std::string& key) {
  return std::unique_ptr<IBackend::IStreamDescriptor>(
      new StreamDescriptor(getStreamRef(key)));
}

// FIXPAUL locking!
std::shared_ptr<StreamRef> FileBackend::getStreamRef(const std::string& key) {
  auto stream_id = getStreamId(key);
  auto iter = stream_refs_.find(stream_id);

  if (iter == stream_refs_.end()) {
    std::shared_ptr<StreamRef> stream_ref(new StreamRef(this, stream_id, key));
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


}
}
