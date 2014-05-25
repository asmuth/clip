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
#include <unordered_map>
#include "../storagebackend.h"
#include "streamref.h"

/**
 * A file-backed, threadsafe storage backend for FnordMetric. This backend
 * multiplexes an arbitrary number of stream into a single log-structured file.
 *
 *
 * eBNF:
 *
 *
    FILE             ::= FILE_HEADER
                         *( PAGE | PADDING )

    FILE_HEADER      ::= MAGIC_BYTES        ; magic bytes
                         VERSION            ; version number
                         PAGE_PTR           ; pointer to first log page

    MAGIC_BYTES      ::= <8 Bytes 0x17>     ; magic bytes
    PADDING          ::= <N Bytes 0x0>      ; zero byte padding
    VERSION          ::= <uint64_t>         ; file version number
    UNIX_MILLIS      ::= <uint64_t>         ; unix millisecond timestamp

    PAGE             ::= ( LOG_PAGE | DATA_PAGE )

    PAGE_HEADER      ::= PAGE_SIZE
                         PAGE_USED
                         PAGE_FLAGS

    PAGE_SIZE        ::= <uint64_t>         ; size of the page in bytes
    PAGE_USED        ::= <uint64_t>         ; number of used byte in the page
    PAGE_FLAGS       ::= <uint64_t>         ; page flags
    PAGE_PTR         ::= <uint64_t>         ; page offset in the file

    LOG_PAGE         ::= PAGE_HEADER
                         *( LOG_ENTRY )

    LOG_ENTRY        ::= ( LOG_ENTRY_NEXT | LOG_ENTRY_ALLOC | LOG_ENTRY_FREE )

    LOG_ENTRY_NEXT   ::= LOG_ENTRY_TYPE     ; 0x01
                         LOG_ENTRY_LENGTH   ; 0x08
                         PAGE_PTR           ; pointer to next log page

    LOG_ENTRY_ALLOC  ::= LOG_ENTRY_TYPE     ; 0x02
                         LOG_ENTRY_LENGTH   ; variable
                         PAGE_PTR           ; pointer to the data page
                         UNIX_MILLIS        ; timestamp of the page's first row
                         STREAM_ID
                         [ STREAM_KEY ]     ; length = LOG_ENTRY_LENGTH - 16

    LOG_ENTRY_FREE   ::= LOG_ENTRY_TYPE     ; 0x03
                         LOG_ENTRY_LENGTH   ; 0x10
                         PAGE_PTR           ; pointer to the free'd page
                         PAGE_SIZE          ; size of the freed page

    DATA_PAGE         ::= PAGE_HEADER
                         *( STREAM_ROW )

    STREAM_ID        ::= <uint64_t>         ; unique stream id
    STREAM_KEY       ::= <string>           ; unique stream key

*/
namespace fnordmetric {
namespace filebackend {

class FileBackend : public IStorageBackend {
  friend class FileBackendTest;
public:
  FileBackend(const FileBackend& copy) = delete;
  FileBackend& operator=(const FileBackend& copy) = delete;

  /**
   * Instantiate a new file backend with a path to the file.
   */
  static std::unique_ptr<FileBackend> openFile(const std::string& filename);

  /**
   * Set the target page size in number of rows. Default: 1024
   */
  void setTargetRowsPerPage(size_t bytes);

  /**
   * Set the maximum allowed page size. Note that now single row may be larger
   * than the default page size. Default: 32Mb
   */
  void setMaxPageSize(size_t bytes);

  /**
   * Retrieve a storage cursor for a stream. See "storagebackend.h" for extended
   * documentation.
   */
  virtual std::unique_ptr<IStorageCursor> getCursor(
      const std::string& key) override;

protected:
  FileBackend();

  /**
   * Retrieve the stream ref for the specified stream id
   */
  std::shared_ptr<StreamRef> getStreamRef(const std::string& key);

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

  /**
   * Highest used stream_id
   */
  uint64_t max_stream_id_;

};

}
}

#endif
