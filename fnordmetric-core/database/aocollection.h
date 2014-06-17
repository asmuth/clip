/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FILEBACKEND_FILEBACKEND_H
#define _FNORDMETRIC_FILEBACKEND_FILEBACKEND_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <mutex>
#include "pagemanager.h"
#include "collection.h"
#include "cursor.h"
#include "transaction.h"
#include "pageindex.h"

/**
 *
 * eBNF:
 *
 *
    FILE             ::= FILE_HEADER
                         *( PAGE )

    FILE_HEADER      ::= <8 Bytes 0x17>     ; magic bytes
                         <uint64_t>         ; version number
                         PAGE_PTR           ; pointer to first log page

    PAGE             ::= ( LOG_PAGE | DATA_PAGE )

    PAGE_PTR         ::= <uint64_t>         ; page offset in the file in bytes
                         <uint32_t>         ; size of the page in bytes

    LOG_PAGE         ::= *( LOG_ENTRY )

    LOG_ENTRY        ::= ( LOG_ENTRY_NEXT | LOG_ENTRY_ALLOC | LOG_ENTRY_FREE )

    LOG_ENTRY_NEXT   ::= LOG_ENTRY_CHECKSUM
                         LOG_ENTRY_TYPE     ; 0x01
                         LOG_ENTRY_LENGTH   ; 0x08
                         PAGE_PTR           ; pointer to next log page

    LOG_ENTRY_ALLOC  ::= LOG_ENTRY_CHECKSUM
                         LOG_ENTRY_TYPE     ; 0x02
                         LOG_ENTRY_LENGTH   ; variable
                         PAGE_PTR           ; pointer to the data page
                         UNIX_MILLIS        ; timestamp of the page's first row
                         STREAM_ID
                         [ STREAM_KEY ]     ; length = LOG_ENTRY_LENGTH - 16

    LOG_ENTRY_FREE   ::= LOG_ENTRY_CHECKSUM
                         LOG_ENTRY_TYPE     ; 0x03
                         LOG_ENTRY_LENGTH   ; 0x10
                         PAGE_PTR           ; pointer to the free'd page
                         PAGE_SIZE          ; size of the freed page

    DATA_PAGE         ::= PAGE_HEADER
                         *( STREAM_ROW )

    UNIX_MILLIS      ::= <uint64_t>         ; unix millisecond timestamp
    STREAM_ID        ::= <uint64_t>         ; unique stream id
    STREAM_KEY       ::= <string>           ; unique stream key

*/
namespace fnordmetric {

class IDSequence;

class AOCollection : public Collection {
public:
  static const uint16_t CollectionTypeId = 0x01;
  static uint64_t kInitialIndexPageSize;

  /**
   * Constructor for a new collection
   */
  AOCollection(
      const Schema& schema,
      std::shared_ptr<PageManager> page_manager,
      std::unique_ptr<IDSequence> seq);

  /**
   * Constructor for an exisiting collection
   */
  AOCollection(
      const Schema& schema,
      PageManager::Page root_page,
      std::shared_ptr<PageManager> page_manager);

  AOCollection(const AOCollection& copy) = delete;
  AOCollection& operator=(const AOCollection& copy) = delete;
  ~AOCollection() override;

  /**
   * Get a snapshot of this collection
   */
  std::unique_ptr<fnordmetric::Collection::Snapshot> getSnapshot() override;

  /**
   * Commit a transaction on this collection. Do not call this directly unless
   * you know what you are doing. Use Transaction#commit instead
   */
  bool commitTransaction(const Transaction* transaction) override;

  /**
   * Sync the log to disk. Makes all changes until this point durable and blocks
   * on what is essentialy an fsync()
   */
  void sync() override;

protected:

  struct __attribute__((__packed__)) DocHeader {
    uint32_t checksum;
    uint32_t size;
    uint64_t key;
    uint8_t data[];
    uint32_t computeChecksum();
  };

  /**
   * Refer to the interface documentation in "collection.h"
   */
  class Snapshot : public fnordmetric::Collection::Snapshot {
  public:
    std::unique_ptr<Cursor> getCursor(const DocumentKey& key) override;
  };

  /**
   * Refer to the interface documentation in "cursor.h"
   */
  class Cursor : public fnordmetric::Cursor {
  public:
    size_t advanceBy(size_t n) override;
  };

  void appendDocument(const DocumentRef* docref,PageIndex* index);
  std::shared_ptr<PageIndex> getPageIndex() const;

  uint64_t last_key_;
  std::shared_ptr<PageIndex> page_index_;
  mutable std::mutex page_index_mutex_;
  std::mutex commit_mutex_;
  std::mutex sync_mutex_;
  std::unique_ptr<IDSequence> seq_;
};

}
#endif
