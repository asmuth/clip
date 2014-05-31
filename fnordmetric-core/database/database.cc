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
#include "database.h"
#include "cursor.h"
#include "streamref.h"
#include "pagemanager.h"
#include "log.h"

namespace fnordmetric {
namespace database {

Database::Database(
    std::shared_ptr<Log> log,
    std::shared_ptr<PageManager> page_manager) :
    log_(std::move(log)),
    page_manager_(std::move(page_manager)),
    max_stream_id_(0) {}

std::unique_ptr<Database> Database::openFile(const std::string& filename) {
  Database* ptr = nullptr;

  int fd = open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    perror("open() failed");
    return std::unique_ptr<Database>(nullptr);
  }

  struct stat fd_stat;
  if (fstat(fd, &fd_stat) < 0) {
    perror("fstat() failed");
    return std::unique_ptr<Database>(nullptr);
  }

  off_t fd_len = lseek(fd, 0, SEEK_END);
  if (fd_len < 0) {
    perror("lseek() failed");
    return std::unique_ptr<Database>(nullptr);
  }

  std::shared_ptr<MmapPageManager> page_manager(
      new MmapPageManager(fd, fd_len, fd_stat.st_blksize));

  /* create new file */
  if (fd_len == 0) {
    auto header_page = page_manager->allocPage(kMinReservedHeaderSize);
    auto header_mmap = page_manager->getPage(header_page);
    auto first_log_page = page_manager->allocPage(kMinLogPageSize);
    auto file_header = header_mmap->structAt<FileHeader>(0);

    file_header->magic = kFileMagicBytes;
    file_header->version = kFileVersion;
    file_header->first_log_page_offset = first_log_page.offset;
    file_header->first_log_page_size = first_log_page.size;
    // FIXPAUL msync header

    std::shared_ptr<Log> log(
        new Log(first_log_page, page_manager));

    return std::unique_ptr<Database>(new Database(log, page_manager));
  }

  /* open existing file */
  if (fd_len >= kMinReservedHeaderSize) {
    PageManager::Page header_page = {.offset=0, .size=kMinReservedHeaderSize};
    auto header_mmap = page_manager->getPage(header_page);
    auto file_header = header_mmap->structAt<FileHeader>(0);

    if (file_header->magic != kFileMagicBytes) {
      fprintf(stderr, "invalid file\n"); // FIXPAUL
      return std::unique_ptr<Database>(nullptr);
    }

    if (file_header->version != kFileVersion) {
      fprintf(stderr, "invalid file version\n"); // FIXPAUL
      return std::unique_ptr<Database>(nullptr);
    }

    PageManager::Page first_log_page;
    first_log_page.offset = file_header->first_log_page_offset;
    first_log_page.size = file_header->first_log_page_size;
    LogReader log_reader(page_manager, first_log_page);
    LogSnapshot log_snapshot;
    log_reader.import(&log_snapshot);

    std::shared_ptr<PageManager> page_manager_imported(new MmapPageManager(
        dup(fd),
        fd_len,
        fd_stat.st_blksize,
        log_snapshot));

    std::shared_ptr<Log> log(
        new Log(log_snapshot, page_manager_imported));

    return std::unique_ptr<Database>(new Database(log, page_manager));
  }

  fprintf(stderr, "invalid file\n"); // FIXPAUL
  return std::unique_ptr<Database>(nullptr);
}

std::unique_ptr<IBackend::IStreamDescriptor> Database::openStream(
    const std::string& key) {
  return std::unique_ptr<IBackend::IStreamDescriptor>(
      new StreamDescriptor(getStreamRef(key)));
}

// FIXPAUL locking!
std::shared_ptr<StreamRef> Database::getStreamRef(const std::string& key) {
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
uint64_t Database::getStreamId(const std::string& stream_key) {
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
