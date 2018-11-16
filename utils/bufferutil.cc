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
#include "bufferutil.h"
#include "inspect.h"

namespace signaltk {

void BufferUtil::stripTrailingBytes(Buffer* buf, unsigned char byte) {
  auto begin = (const unsigned char*) buf->data();
  auto cur = begin + buf->size();

  while (cur > begin && *(cur - 1) == byte) {
    cur--;
  }

  buf->truncate(cur - begin);
}

void BufferUtil::stripTrailingSlashes(Buffer* buf) {
  stripTrailingBytes(buf, '/');
}

std::string BufferUtil::hexPrint(
    Buffer* buf,
    bool sep /* = true */,
    bool reverse /* = fase */) {
  static const char hexTable[] = "0123456789abcdef";
  auto data = (const unsigned char*) buf->data();
  auto size = buf->size();
  std::string str;

  if (reverse) {
    for (int i = size - 1; i >= 0; --i) {
      if (sep && i < size - 1) { str += " "; }
      auto byte = data[i];
      str += hexTable[(byte & 0xf0) >> 4];
      str += hexTable[byte & 0x0f];
    }
  } else {
    for (int i = 0; i < size; ++i) {
      if (sep && i > 0) { str += " "; }
      auto byte = data[i];
      str += hexTable[(byte & 0xf0) >> 4];
      str += hexTable[byte & 0x0f];
    }
  }

  return str;
}
} // namespace signaltk

