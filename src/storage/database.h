/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_FILEBACKEND_FILEBACKEND_H
#define _FNORDMETRIC_FILEBACKEND_FILEBACKEND_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "pagemanager.h"
#include "log.h"

/**
 * A storage backend stores an arbitrary number of 'streams'. A stream consists
 * of rows. Each row is a <time, data> tuple where time is the time at which the
 * row was inserted into the stream and data is a binary string. Streams are
 * append only. Each stream is identified by a unique string key.
 *
 * A file-backed, threadsafe storage backend for FnordMetric. This backend
 * multiplexes an arbitrary number of stream into a single log-structured file.
 *
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
namespace query {
class Query;
}
namespace database {
class StreamRef;

enum kDatabaseFlags {
  /**
   * Safety modes. The database will be consistent in all cases, but depending
   * on the safety mode you might loose more or less data.
   */
  MODE_RELAXED = 0,
  MODE_CONSERVATIVE = 10, /* MSYNC_ASYNC | ENABLE_ROW_CHECKSUMS */
  MODE_PARANOID = 12, /* MSYNC_SYNC | ENABLE_ROW_CHECKSUMS */

  /**
   * Msync modes
   */
  MYSNC_ASYNC = 2,
  MYSNC_SYNC = 4,

  /**
   * Verify checksums when scanning over rows. Slows down everthing by a bit
   * but protects against file corruption
   */
  ENABLE_ROW_CHECKSUMS = 8,

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

class Database {
  friend class StreamRef;
  friend class Cursor;
  friend class DatabaseTest;
public:
  struct __attribute__((__packed__)) FileHeader {
    uint64_t magic;
    uint64_t version;
    uint64_t first_log_page_offset;
    uint64_t first_log_page_size;
  };

  Database(const Database& copy) = delete;
  Database& operator=(const Database& copy) = delete;
  ~Database();

  /**
   * Min. number of bytes to reserve for the file header
   */
  static const uint64_t kMinReservedHeaderSize = 512;

  /**
   * File magic bytes
   */
  static const uint64_t kFileMagicBytes = 0x1717171717171717;

  /**
   * File format version number
   */
  static const uint64_t kFileVersion = 1;

  /**
   * Target page size in number of rows. Default: 16384 rows
   */
  static size_t kTargetRowsPerPage;

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
   * Instantiate a new file backend with a path to the file.
   */
  static std::unique_ptr<Database> openFile(
      const std::string& filename,
      uint64_t flags = MODE_CONSERVATIVE);

  /**
   * Execute a query
   */
  void executeQuery(query::Query* query);

  /**
   * Open or create the stream with the specified key.
   * This method is threadsafe.
   */
  std::shared_ptr<StreamRef> openStream(const std::string& key);

protected:
  Database(
      LogSnapshot& log_snapshot,
      std::shared_ptr<Log> log,
      std::shared_ptr<PageManager> page_manager,
      uint64_t flags);

  Database(
      std::shared_ptr<Log> log,
      std::shared_ptr<PageManager> page_manager,
      uint64_t flags);

  /**
   * Retrieve the stream id for a specified string stream key
   */
  uint64_t getStreamId(const std::string& key);

  /**
   * Maps string stream keys to stream ids
   */
  std::unordered_map<std::string, uint64_t> stream_ids_;

  /**
   * Holds all the StreamRefs
   */
  std::unordered_map<uint64_t, std::shared_ptr<StreamRef>> stream_refs_;
  std::mutex stream_refs_mutex_;

  /**
   * Highest used stream_id
   */
  uint64_t max_stream_id_;

  const std::shared_ptr<Log> log_;
  const std::shared_ptr<PageManager> page_manager_;
  uint64_t flags_;
};

}
}

#endif
