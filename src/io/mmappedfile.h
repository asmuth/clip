/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_IO_MMAPPED_FILE_H_
#define _FNORD_IO_MMAPPED_FILE_H_
#include <fnordmetric/io/file.h>
#include <fnordmetric/util/runtimeexception.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace io {

class MmappedFile {
public:
  MmappedFile() = delete;
  MmappedFile(File&& file);
  MmappedFile(const MmappedFile& copy) = delete;
  MmappedFile& operator=(const MmappedFile& copy) = delete;

  inline void* data() const {
    return data_;
  }

  inline size_t size() const {
    return size_;
  }

  template <typename T>
  inline T* structAt(size_t pos) const {
#ifndef NDEBUG
    if (pos >= size_) {
      RAISE(kIndexError, "position out of bounds");
    }
#endif

    return (T*) (((char *) data_) + pos);
  }

  bool isWritable() const;

protected:
  File file_;
  void* data_;
  size_t size_;
};

}
}
#endif
