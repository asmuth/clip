/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/util/buffer.h>
#include <metricd/util/exception.h>
#include <metricd/util/file.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

File File::openFile(
    const std::string& filename,
    int flags,
    int permissions /* = 0666 */) {
  int open_flags = 0;

  switch (flags & (O_READ | O_WRITE)) {
    case O_READ:
      open_flags = O_RDONLY;
      break;

    case O_WRITE:
      open_flags = O_WRONLY;
      break;

    case (O_READ | O_WRITE):
      open_flags = O_RDWR;
      break;

    default:
      RAISE(
          kIllegalArgumentError,
          "openFile('%s'): at least one of O_READ | O_WRITE must be set",
          filename.c_str());
  }

  if ((flags & O_CREATEOROPEN) > 0) {
    open_flags |= O_CREAT;
  } else if ((flags & O_CREATE) > 0) {
    open_flags |= O_CREAT;
    open_flags |= O_EXCL;
  }

  if ((flags & O_TRUNCATE) > 0) {
    open_flags |= O_TRUNC;
  }

  int fd = open(filename.c_str(), open_flags, permissions);

  if (fd < 0) {
    RAISE_ERRNO(kIOError, "openFile('%s'): open() failed", filename.c_str());
  }

  if ((flags & O_AUTODELETE) > 0) {
    unlink(filename.c_str());
  }

  if ((flags & O_APPEND) > 0) {
    lseek(fd, 0, SEEK_END);
  }

  return File(fd, flags);
}

File::File(int fd, int flags) : fd_(fd), flags_(flags) {}

File::~File() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

File::File(File&& other) : fd_(other.fd_), flags_(other.flags_) {
  other.fd_ = -1;
  other.flags_ = 0;
}

File& File::operator=(File&& other) {
  if (fd_ >= 0) {
    close(fd_);
  }

  fd_ = other.fd_;
  flags_ = other.flags_;
  other.fd_ = -1;
  other.flags_ = 0;
  return *this;
}

int File::fd() const {
  return fd_;
}

int File::releaseFD() {
  auto fd = fd_;
  fd_ = -1;
  return fd;
}

bool File::isWritable() const {
  return (flags_ & O_WRITE) == O_WRITE;
}

size_t File::size() const {
  struct stat fd_stat;

  if (fstat(fd_, &fd_stat) < 0) {
    RAISE_ERRNO(kIOError, "fstat(%i) failed", fd_);
  }

  return fd_stat.st_size;
}

void File::seekTo(size_t pos) {
  if (lseek(fd_, pos, SEEK_SET) < 0) {
    RAISE_ERRNO(kIOError, "lseek(%i) failed", fd_);
  }
}

size_t File::read(void* buf, size_t buf_len) {
  int res = ::read(fd_, buf, buf_len);

  if (res < 0) {
    RAISE_ERRNO(kIOError, "read(%i) failed", fd_);
  }

  return res;
}

size_t File::read(Buffer* buf) {
  return read(buf->data(), buf->size());
}

void File::write(const void* buf, size_t buf_len) {
  int res = ::write(fd_, buf, buf_len);

  if (res < 0 || res != buf_len) {
    RAISE_ERRNO(kIOError, "write(%i) failed", fd_);
  }
}

void File::write(const Buffer& buf) {
  write(buf.data(), buf.size());
}

void File::write(const String& buf) {
  write(buf.c_str(), buf.length());
}

void File::pwrite(uint64_t position, const void* buf, size_t buf_len) {
  int res = ::pwrite(fd_, buf, buf_len, position);

  if (res < 0 || res != buf_len) {
    RAISE_ERRNO(kIOError, "write(%i) failed", fd_);
  }
}

void File::pwrite(uint64_t position, const Buffer& buf) {
  pwrite(position, buf.data(), buf.size());
}

void File::pwrite(uint64_t position, const String& buf) {
  pwrite(position, buf.c_str(), buf.length());
}

File File::clone() const {
  int new_fd = dup(fd_);

  if (new_fd < 0) {
    RAISE_ERRNO(kIOError, "dup(%i) failed", fd_);
  }

  return File(new_fd, flags_);
}

void File::truncate(size_t new_size) {
  if (ftruncate(fd_, new_size) < 0) {
    RAISE_ERRNO(kIOError, "ftruncate(%i) failed", fd_);
  }
}

void File::fsync(bool sync_metadata /* = false */) {
  int retval;

  if (sync_metadata) {
    retval = ::fsync(fd_);
  } else {
#if _POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE >= 500
    retval = ::fdatasync(fd_);
#else
    retval = ::fsync(fd_);
#endif
  }

  if (retval != 0) {
    RAISE_ERRNO(kIOError, "fsync(%i) failed", fd_);
  }
}

