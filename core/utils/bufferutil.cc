/**
 * This file is part of the "fviz" project
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
#include "bufferutil.h"
#include "inspect.h"

namespace fviz {

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
} // namespace fviz

