/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include "fnordmetric/util/bufferutil.h"
#include "fnordmetric/util/inspect.h"

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

