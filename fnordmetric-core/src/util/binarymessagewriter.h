/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_UTIL_BINARYMESSAGEWRITER_H
#define _FNORD_UTIL_BINARYMESSAGEWRITER_H
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace util {

class BinaryMessageWriter {
public:
  static const size_t kInitialDataSize = 4096;

  BinaryMessageWriter(size_t initial_size = kInitialDataSize);
  BinaryMessageWriter(void* buf, size_t buf_len);
  ~BinaryMessageWriter();

  void appendUInt16(uint16_t value);
  void appendUInt32(uint32_t value);
  void appendUInt64(uint64_t value);
  void appendString(const std::string& string);
  void append(void const* data, size_t size);

  void updateUInt16(size_t offset, uint16_t value);
  void updateUInt32(size_t offset, uint32_t value);
  void updateUInt64(size_t offset, uint64_t value);
  void updateString(size_t offset, const std::string& string);
  void update(size_t offset, void const* data, size_t size);

  void* data() const;
  size_t size() const;

protected:
  void* ptr_;
  size_t size_;
  size_t used_;
  bool owned_;
};

}
}

#endif
