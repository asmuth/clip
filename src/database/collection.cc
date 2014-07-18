/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "collection.h"
#include "pagemanager.h"
#include "transaction.h"
#include "snapshot.h"

namespace fnordmetric {

Collection::Collection(
    std::shared_ptr<PageManager> page_manager,
    std::unique_ptr<IDSequence> seq) :
    page_manager_(std::move(page_manager)) {}


Collection::~Collection() {}

std::unique_ptr<Transaction> Collection::startTransaction() {
  std::unique_ptr<Snapshot> snapshot(new Snapshot());
  auto transaction = new Transaction(std::move(snapshot));
  return std::unique_ptr<Transaction>(transaction);
}

void Collection::writeRootPage(const PageManager::Page& root_page) {
  PageManager::Page header_page;
  header_page.offset = 0;
  header_page.size = kMinReservedHeaderSize;

  auto header_mmap = page_manager_->getPage(header_page);
  auto file_header = header_mmap->structAt<FileHeader>(0);

  file_header->root_page_offset = root_page.offset;
  file_header->root_page_size = root_page.size;
  // FIXPAUL msync header
}

std::shared_ptr<PageManager> Collection::getPageManager() const {
  return page_manager_;
}

std::unique_ptr<Collection> Collection::createPersistentCollection(
    const std::string& filename,
    uint64_t flags /* = MODE_CONSERVATIVE */) {
  Collection* ptr = nullptr;

  auto open_flags = O_CREAT | O_RDWR;
  if (flags & FILE_TRUNCATE) {
    flags |= O_TRUNC;
  } else {
    flags |= O_EXCL;
  }

  int fd = open(filename.c_str(), open_flags, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    perror("create file with open() failed");
    return std::unique_ptr<Collection>(nullptr);
  }

  struct stat fd_stat;
  if (fstat(fd, &fd_stat) < 0) {
    perror("fstat() failed");
    return std::unique_ptr<Collection>(nullptr);
  }
/*
  off_t fd_len = lseek(fd, 0, SEEK_END);
  if (fd_len < 0) {
    perror("lseek() failed");
    return std::unique_ptr<Database>(nullptr);
  }

*/

  if ((flags & FILE_AUTODELETE) > 0) {
    unlink(filename.c_str());
  }

  std::shared_ptr<MmapPageManager> page_manager(
      new MmapPageManager(fd, 0, fd_stat.st_blksize));

  auto header_page = page_manager->allocPage(kMinReservedHeaderSize);
  auto header_mmap = page_manager->getPage(header_page);
  auto file_header = header_mmap->structAt<FileHeader>(0);

  file_header->magic = kFileMagicBytes;
  file_header->version = kFileVersion;
  //file_header->collection_type = T::CollectionTypeId;
  file_header->root_page_offset = 0;
  file_header->root_page_size = 0;
  // FIXPAUL msync header

  std::unique_ptr<IDSequence> seq(new UnixMillisecondIDSequence());
  return std::unique_ptr<Collection>(new Collection(page_manager, std::move(seq)));

  /* open existing file */
  /*if (fd_len >= kMinReservedHeaderSize) {
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
    LogSnapshot log_snapshot;
    LogReader log_reader(page_manager, first_log_page, &log_snapshot);
    log_reader.import();

    std::shared_ptr<PageManager> page_manager_imported(new MmapPageManager(
        dup(fd),
        fd_len,
        fd_stat.st_blksize,
        log_snapshot));

    std::shared_ptr<Log> log(
        new Log(log_snapshot, page_manager_imported));

    return std::unique_ptr<Database>(
        new Database(log_snapshot, log, page_manager_imported, flags));
  }

  fprintf(stderr, "invalid file\n"); // FIXPAUL
  return std::unique_ptr<Database>(nullptr);
  */
}

bool Collection::commitTransaction(const Transaction* transaction) {

}

void Collection::sync() {
  //sync_mutex_.lock();

  // capture free page list
  //auto index = getPageIndex();
  //page_manager_->fsync();
  //writeRootPage(index_page);

  // release freed pages
  //sync_mutex_.unlock();
}

}
