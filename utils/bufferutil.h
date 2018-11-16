/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FNORD_BASE_BUFFERUTIL_H_
#define _FNORD_BASE_BUFFERUTIL_H_

#include <stdlib.h>
#include <stdint.h>
#include "buffer.h"

namespace signaltk {

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

} // namespace signaltk

#endif
