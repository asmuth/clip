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
#ifndef _FNORD_UTIL_BUFFER_H_
#define _FNORD_UTIL_BUFFER_H_
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include "stdtypes.h"

namespace plotfx {

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
