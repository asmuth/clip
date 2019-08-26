/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _FNORD_BASE_BUFFERUTIL_H_
#define _FNORD_BASE_BUFFERUTIL_H_

#include <stdlib.h>
#include <stdint.h>
#include "buffer.h"

namespace clip {

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

} // namespace clip

#endif
