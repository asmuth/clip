/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_FFS_VOLUME_H_
#define _FNORDMETRIC_FFS_VOLUME_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "pagemanager.h"

/**
 * A ffs volume stores an arbitrary number of 'objects'. Each object is
 * identified by a unique string key. The ffs volume itself only implements
 * simple inserting, deleting and accessing objects.
 *
 * Each object has an associated type that specifies the methods to manipulate
 * the type.
 *
 * eBNF:
 *
 *    FILE             ::= FILE_HEADER
 *                         *( PAGE )
 *
 *    PAGE_PTR         ::= <uint64_t>         ; page offset in the file in bytes
 *                         <uint32_t>         ; size of the page in bytes
 *
 *    PAGE             ::= <bytes>            ; opaque byte string
 *
 *    FILE_HEADER      ::= <8 Bytes 0x17>     ; magic bytes
 *                         <uint32_t>         ; version number
 *                         <uint16_t>         ; index type
 *                         PAGE_PTR           ; pointer to index super page
 *                         PAGE_PTR           ; pointer to first FREELIST_PAGE
 *
 */
namespace fnordmetric {
namespace ffs {

class Volume {
public:
  enum kFlags {
    /**
     * Safety modes. The database will be consistent in all cases, but depending
     * on the safety mode you might loose more or less data.
     */
    MODE_RELAXED = 0,
    MODE_CONSERVATIVE = 10, /* MSYNC_ASYNC | ENABLE_CHECKSUMS */
    MODE_PARANOID = 12, /* MSYNC_SYNC | ENABLE_CHECKSUMS */

    /**
     * Msync modes
     */
    MYSNC_ASYNC = 2,
    MYSNC_SYNC = 4,

    /**
     * Verify checksums when scanning over rows. Slows down everthing by a bit
     * but protects against file corruption
     */
    ENABLE_CHECKSUMS = 8,

    /**
     * Truncate the file when opening it
     */
    FILE_TRUNCATE = 32,

    /**
     * Delete the file when the database is closed (the volume object is
     * destroyed)
     */
    FILE_AUTODELETE = 64
  };

  struct __attribute__((__packed__)) PagePtr {
    uint64_t offset;
    uint16_t blks;
  };

  struct __attribute__((__packed__)) ObjectPtr {
    PagePtr super_page;
    uint16_t objtype;
  };

  struct __attribute__((__packed__)) FileHeader {
    uint64_t magic;
    uint32_t version;
    uint32_t block_size;
    ObjectPtr index;
    ObjectPtr freelist;
  };

  Volume(const Volume& copy) = delete;
  Volume& operator=(const Volume& copy) = delete;
  ~Volume();

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
   * Instantiate a new volume with a path to the file.
   */
  static std::unique_ptr<Volume> openFile(
      const std::string& filename,
      uint64_t flags = MODE_CONSERVATIVE);


protected:
  Volume(const std::shared_ptr<PageManager>& page_manager, int flags);

  const std::shared_ptr<PageManager> page_manager_;
  uint64_t flags_;
};

}
}

#endif
