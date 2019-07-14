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
#ifndef _FNORD_UTIL_BUFFER_H_
#define _FNORD_UTIL_BUFFER_H_
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include "stdtypes.h"

namespace fviz {

class Buffer {
public:
  static const size_t npos = -1;

  Buffer();
  Buffer(const void* initial_data, size_t initial_size);
  Buffer(size_t initial_size);
  Buffer(const Buffer& copy);
  Buffer(Buffer&& move);
  Buffer& operator=(const Buffer& copy);
  Buffer& operator=(Buffer&& move);
  ~Buffer();

  bool operator==(const Buffer& buf) const;
  bool operator==(const char* str) const;
  bool operator==(const std::string& str) const;

  void append(const void* data, size_t size);
  void append(const String& string);
  void clear();
  void truncate(size_t size);

  void* data() const;
  char charAt(size_t pos) const;
  size_t find(char chr) const;
  size_t size() const;
  size_t allocSize() const;
  std::string toString() const;

  void reserve(size_t size);

  void setMark(size_t mark);
  size_t mark() const;

protected:
  void* data_;
  size_t size_;
  size_t alloc_;
  size_t mark_;
};

}
#endif
