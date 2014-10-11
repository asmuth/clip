/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_COLLECTION_H
#define _FNORDMETRIC_COLLECTION_H

#include <stdlib.h>
#include <stdint.h>
#include "pagemanager.h"
#include "idsequence.h"

namespace fnordmetric {
class Transaction;
class Cursor;
class DocumentKey;

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
  static std::unique_ptr<Collection> createPersistentCollection(
      const std::string& filename,
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
   * Sync the log to disk. Makes all changes until this point durable and blocks
   * on what is essentialy an fsync()
   */
  virtual void sync();

  /**
   * Get the internal page manager for this collection. Do not call unless you
   * know what you are doing
   */
  std::shared_ptr<PageManager> getPageManager() const;

  /**
   * Commit a transaction on this collection. Do not call this directly unless
   * you know what you are doing. Use Transaction#commit instead
   */
  virtual bool commitTransaction(const Transaction* transaction);

protected:

  /**
   * Constructor for a new collection
   */
  explicit Collection(
      std::shared_ptr<PageManager> page_manager,
      std::unique_ptr<IDSequence> id_sequence);

  /**
   * Constructor for an exisiting collection
   */
  explicit Collection(
      PageManager::Page root_page,
      std::shared_ptr<PageManager> page_manager);

  void writeRootPage(const PageManager::Page& root_page);

  const std::shared_ptr<PageManager> page_manager_;
};

}
#endif
