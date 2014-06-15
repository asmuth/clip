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

class AOCollection : public Collection {
public:
  AOCollection(const AOCollection& copy) = delete;
  AOCollection& operator=(const AOCollection& copy) = delete;
  ~AOCollection();

  /**
   * Target page size in number of rows. Default: 16384 rows
   */
  static size_t kTargetRowsPerPage;

  /**
   * Start a new transaction on this collection
   */
  std::unique_ptr<Transaction> startTransaction() override;

};

}
#endif
