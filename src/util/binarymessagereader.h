/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_UTIL_BINARYMESSAGEREADER_H
#define _FNORD_UTIL_BINARYMESSAGEREADER_H
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace util {

class BinaryMessageReader {
public:
  BinaryMessageReader(void* buf, size_t buf_len);

  void appendUInt32(uint32_t value);
  void appendUInt64(uint64_t value);
  void appendString(const std::string& string);
  void append(void const* data, size_t size);

  void rewind();

protected:
  void* ptr_;
  size_t size_;
  size_t pos_;
};

}
}

#endif
