/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <sys/mman.h>
#include <unistd.h>
#include <fnord/base/io/mmappedfile.h>

namespace fnord {
namespace io {

MmappedFile::MmappedFile(File&& file) {
  File local_file = std::move(file);

  size_ = local_file.size();
  is_writable_ = local_file.isWritable();

  if (size_ == 0) {
    RAISE(kIllegalArgumentError, "can't mmap() empty file");
  }

  data_ = mmap(
      nullptr,
      size_,
      is_writable_ ? PROT_WRITE | PROT_READ : PROT_READ,
      MAP_SHARED,
      local_file.fd(),
      0);

  if (data_ == MAP_FAILED) {
    RAISE_ERRNO(kIOError, "mmap() failed");
  }
}

MmappedFile::~MmappedFile() {
  munmap(data_, size_);
}

bool MmappedFile::isWritable() const {
  return is_writable_;
}

}
}
