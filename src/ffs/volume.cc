/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/ffs/volume.h>
#include <fnordmetric/util/runtimeexception.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace fnordmetric {
namespace ffs {

Volume::Volume(
    const std::shared_ptr<io::PageManager>& page_manager,
    int flags) :
    page_manager_(page_manager),
    flags_(flags) {}

Volume::~Volume() {
}

std::unique_ptr<Volume> Volume::openFile(
    const std::string& filename,
    uint64_t flags /* = MODE_CONSERVATIVE */) {
  Volume* ptr = nullptr;

  int open_flags = O_CREAT | O_RDWR;
  if ((flags & FILE_TRUNCATE) > 0) {
    open_flags |= O_TRUNC;
  }

  int fd = open(filename.c_str(), open_flags, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    RAISE_ERRNO(kIOError, "open() failed");
  }

  struct stat fd_stat;
  if (fstat(fd, &fd_stat) < 0) {
    RAISE_ERRNO(kIOError, "fstat() failed");
  }

  off_t fd_len = lseek(fd, 0, SEEK_END);
  if (fd_len < 0) {
    RAISE_ERRNO(kIOError, "lseek() failed");
  }

  if ((flags & FILE_AUTODELETE) > 0) {
    unlink(filename.c_str());
  }

  std::shared_ptr<io::MmapPageManager> page_manager(
      new io::MmapPageManager(fd, fd_len, fd_stat.st_blksize));

  io::PageManager::Page header_page;
  if (fd_len == 0) {
    header_page = page_manager->allocPage(kMinReservedHeaderSize);
  } else {
    if (fd_len < kMinReservedHeaderSize) {
      RAISE(kIllegalFormatError, "invalid file\n");
    }

    header_page = io::PageManager::Page(0, kMinReservedHeaderSize);
  }

  auto header_mmap = page_manager->getPage(header_page);
  auto file_header = header_mmap->structAt<FileHeader>(0);

  if (fd_len == 0) {
    file_header->magic = kFileMagicBytes;
    file_header->version = kFileVersion;
    // FIXPAUL msync header
  } else {
    if (file_header->magic != kFileMagicBytes) {
      RAISE(kIllegalFormatError, "invalid file\n");
    }

    if (file_header->version != kFileVersion) {
      RAISE(kVersionMismatchError, "invalid file version\n");
    }
  }

  return std::unique_ptr<Volume>(new Volume(page_manager, flags));
}

}
}

