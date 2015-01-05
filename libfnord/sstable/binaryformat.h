/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_SSTABLE_H
#define _FNORD_SSTABLE_SSTABLE_H
#include <stdlib.h>
#include <stdint.h>

namespace fnord {
namespace sstable {

/**
 * // http://tools.ietf.org/html/rfc5234
 *
 *   <sstable> :=
 *       <header>
 *       <body>
 *       *<footer>
 *
 *   <header> :=
 *       %x17 %x17 %x17 %x17"    // magic bytes
 *       %x00 %x01               // sstable file format version
 *       <uint64_t>              // total body size in bytes
 *       <uint32_t>              // userdata checksum
 *       <uint32_t>              // userdata size in bytes
 *       <bytes>                 // userdata
 *
 *   <body> :=
 *       *<row>
 *
 *   <row> :=
 *       <uint32_t>              // row checksum
 *       <uint32_t>              // key size in bytes
 *       <uint32_t>              // data size in bytes
 *       <bytes>                 // key
 *       <bytes>                 // data
 *
 *   <footer> :=
 *       %x17 %x17 %x17 %x17"    // magic bytes
 *       <uint32_t>              // footer type id
 *       <uint32_t>              // userdata size in bytes
 *       <bytes>                 // userdata
 *
 */
class BinaryFormat {
public:
  static const uint16_t kVersion = 1;
  static const uint64_t kMagicBytes = 0x17171717;

  struct __attribute__((packed)) RowHeader {
    uint32_t checksum;
    uint32_t key_size;
    uint32_t data_size;
  };

  struct __attribute__((packed)) FooterHeader {
    uint64_t magic;
    uint32_t type;
    uint32_t footer_checksum;
    uint32_t footer_size;
  };

};

}
}

#endif
