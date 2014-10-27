/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/io/mmappedfile.h>
#include <sys/mman.h>
#include <unistd.h>

namespace fnord {
namespace io {

MmappedFile::MmappedFile(File&& file) : file_(std::move(file)) {
  size_ = file_.size();

  if (size_ == 0) {
    RAISE(kIllegalArgumentError, "can't mmap() empty file");
  }

  data_ = mmap(
      nullptr,
      size_,
      file_.isWritable() ? PROT_WRITE | PROT_READ : PROT_READ,
      MAP_SHARED,
      file_.fd(),
      0);

  if (data_ == MAP_FAILED) {
    RAISE_ERRNO(kIOError, "mmap() failed");
  }
}

bool MmappedFile::isWritable() const {
  return file_.isWritable();
}

}
}
