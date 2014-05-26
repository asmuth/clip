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
#include "log.h"

namespace fnordmetric {
namespace filebackend {

FileBackend::FileBackend(
    std::shared_ptr<Log> log,
    std::shared_ptr<PageManager> page_manager,
    std::shared_ptr<MmapPageManager> mmap_manager) :
    log_(std::move(log)),
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

  std::shared_ptr<MmapPageManager> mmap_manager(
      new MmapPageManager(fd, fd_len));

  /* create new file */
  if (fd_len == 0) {
    std::shared_ptr<PageManager> page_manager(
        new PageManager(0, fd_stat.st_blksize));
    auto header_page = page_manager->allocPage(kMinReservedHeaderSize);
    auto header_mmap = mmap_manager->getPage(header_page);

    auto first_log_page = page_manager->allocPage(kMinLogPageSize);
    auto file_header = header_mmap.structAt<FileHeader>(0);
    file_header->magic = kFileMagicBytes;
    file_header->version = kFileVersion;
    file_header->first_log_page_offset = first_log_page.offset;
    file_header->first_log_page_size = first_log_page.size;
    // FIXPAUL msync header

    std::shared_ptr<Log> log(
        new Log(first_log_page, page_manager, mmap_manager));

    auto backend = new FileBackend(log, page_manager, mmap_manager);
    return std::unique_ptr<FileBackend>(backend);
  }

  /* open existing file */
  if (fd_len >= kMinReservedHeaderSize) {
    PageManager::Page header_page = {.offset=0, .size=kMinReservedHeaderSize};
    auto header_mmap = mmap_manager->getPage(header_page);
    auto file_header = header_mmap.structAt<FileHeader>(0);

    if (file_header->magic != kFileMagicBytes) {
      fprintf(stderr, "invalid file\n"); // FIXPAUL
      return std::unique_ptr<FileBackend>(nullptr);
    }

    if (file_header->version != kFileVersion) {
      fprintf(stderr, "invalid file version\n"); // FIXPAUL
      return std::unique_ptr<FileBackend>(nullptr);
    }

    PageManager::Page first_log_page;
    first_log_page.offset = file_header->first_log_page_offset;
    first_log_page.size = file_header->first_log_page_size;
    Log::Snapshot log_snapshot;
    Log::import(mmap_manager, first_log_page, &log_snapshot);

    std::shared_ptr<PageManager> page_manager(
        new PageManager(log_snapshot.last_used_byte, fd_stat.st_blksize));

    std::shared_ptr<Log> log(
        new Log(log_snapshot, page_manager, mmap_manager));

    auto backend = new FileBackend(log, page_manager, mmap_manager);
    return std::unique_ptr<FileBackend>(backend);
  }

  fprintf(stderr, "invalid file\n"); // FIXPAUL
  return std::unique_ptr<FileBackend>(nullptr);
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
