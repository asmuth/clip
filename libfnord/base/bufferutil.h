/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_BUFFERUTIL_H_
#define _FNORD_BASE_BUFFERUTIL_H_

#include <stdlib.h>
#include <stdint.h>
#include "fnord/base/buffer.h"

namespace fnord {

class BufferUtil {
public:

  /**
   * Remove trailing bytes from the pointed to buffer
   *
   * @param str the string to remove trailing slashes from
   */
  static void stripTrailingBytes(Buffer* buf, unsigned char byte);

  /**
   * Remove trailing slashes from the pointed to buffer
   *
   * @param str the string to remove trailing slashes from
   */
  static void stripTrailingSlashes(Buffer* buf);

  /**
   * Print the contents of the pointed to memory as a series of hexadecimal
   * bytes (hexdump):
   *
   * Example:
   *   StringUtil::hexPrint("\x17\x23\x42\x01", 4);
   *   // returns "17 23 42 01"
   *
   * @param data the data to print
   * @param size the size of the data in bytes
   * @param separate_bytes if true, insert a whitespace character between bytes
   * @param reverse_byte_order if true, print the data from last to first byte
   * @return the data formatted as a human readable hex string
   */
  static std::string hexPrint(
      Buffer* buf,
      bool separate_bytes = true,
      bool reverse_byte_order = false);

};

} // namespace fnord

#endif
