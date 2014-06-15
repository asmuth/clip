/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_COLLECTION_H
#define _FNORDMETRIC_COLLECTION_H

#include <stdlib.h>
#include <stdint.h>
#include "schema.h"
#include "pagemanager.h"

namespace fnordmetric {
class Transaction;
class Cursor;
typedef uint64_t DocumentKey;

/**
 * All methods on a collection instance are threadsafe.
 */
class Collection {
public:
  struct __attribute__((__packed__)) FileHeader {
    uint64_t magic;
    uint64_t version;
    uint64_t root_page_offset;
    uint64_t root_page_size;
    uint16_t collection_type;
  };

  enum kCollectionFlags {
    /**
     * Truncate the file when opening it
     */
    FILE_TRUNCATE = 32,

    /**
     * Delete the file when the database is closed (the database object is
     * destroyed)
     */
    FILE_AUTODELETE = 64
  };

  class Snapshot {

    /**
     * Return a cursor for this snapshot and seek to the first document with a
     * key larger than or equal to the specified key.
     */
    virtual std::unique_ptr<Cursor> getCursor(const DocumentKey& key) = 0;

  };

  /**
   * File magic bytes
   */
  static const uint64_t kFileMagicBytes = 0x1717171717171717;

  /**
   * File format version number
   */
  static const uint64_t kFileVersion = 1;

  /**
   * Min. number of bytes to reserve for the file header
   */
  static const uint64_t kMinReservedHeaderSize = 512;

  /**
   * Preferred maximum page size (soft limit). Default: 4MB
   */
  static size_t kMaxPageSizeSoft;

  /**
   * Maximum allowed page size (hard limit). Note that now single row may be
   * larger than the maximum page size. Default: 32MB
   */
  static size_t kMaxPageSizeHard;

  /**
   * Create a file backed collection
   */
  template<typename T>
  static std::unique_ptr<Collection> createPersistentCollection(
      const std::string& filename,
      const Schema& schema,
      uint64_t flags);

  /**
   * Open a file backed collection
   */
  static std::unique_ptr<Collection> openPersistentCollection(
      const std::string& filename,
      uint64_t flags);

  Collection(const Collection& copy) = delete;
  Collection& operator=(const Collection& copy) = delete;
  virtual ~Collection();

  /**
   * Start a new transaction on this collection
   */
  std::unique_ptr<Transaction> startTransaction();

  /**
   * Commit a transaction on this collection. Do not call this directly unless
   * you know what you are doing. Use Transaction#commit instead
   */
  virtual bool commitTransaction(const Transaction* transaction) = 0;

  /**
   * Get a snapshot of this collection
   */
  virtual std::unique_ptr<Snapshot> getSnapshot() = 0;

  /**
   * Sync the log to disk. Makes all changes until this point durable and blocks
   * on what is essentialy an fsync()
   */
  virtual void sync() = 0;

  /**
   * Get the document schema for this collection
   */
  const Schema& getSchema() const;

  /**
   * Get the internal page manager for this collection. Do not call unless you
   * know what you are doing
   */
  std::shared_ptr<PageManager> getPageManager() const;

protected:

  /**
   * Constructor for a new collection
   */
  explicit Collection(
      const Schema& schema,
      std::shared_ptr<PageManager> page_manager);

  /**
   * Constructor for an exisiting collection
   */
  explicit Collection(
      PageManager::Page root_page,
      std::shared_ptr<PageManager> page_manager);

  void writeRootPage(const PageManager::Page& root_page);

  const Schema& schema_;
  const std::shared_ptr<PageManager> page_manager_;
};

#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

template<typename T>
std::unique_ptr<Collection> Collection::createPersistentCollection(
    const std::string& filename,
    const Schema& schema,
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
  file_header->collection_type = T::CollectionTypeId;
  file_header->root_page_offset = 0;
  file_header->root_page_size = 0;
  // FIXPAUL msync header

  return std::unique_ptr<Collection>(new T(schema, page_manager));

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

}
#endif
