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
#include <cstdarg>
#include <fcntl.h>
#include <memory>
#include <stdarg.h>
#include <string>
#include <unistd.h>
#include "buffer.h"
#include "exception.h"
#include "outputstream.h"

namespace signaltk {

std::unique_ptr<OutputStream> OutputStream::getStdout() {
  auto stdout_stream = new FileOutputStream(1, false);
  return std::unique_ptr<OutputStream>(stdout_stream);
}

std::unique_ptr<OutputStream> OutputStream::getStderr() {
  auto stderr_stream = new FileOutputStream(2, false);
  return std::unique_ptr<OutputStream>(stderr_stream);
}

size_t OutputStream::write(const std::string& data) {
  return write(data.c_str(), data.size());
}

size_t OutputStream::write(const Buffer& buf) {
  return write((const char*) buf.data(), buf.size());
}

// FIXPAUL: variable size buffer
size_t OutputStream::printf(const char* format, ...) {
  char buf[8192];

  va_list args;
  va_start(args, format);
  int pos = vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);

  if (pos < 0) {
    RAISE_ERRNO(kIOError, "vsnprintf() failed");
  }

  if (pos < sizeof(buf)) {
    write(buf, pos);
  } else {
    RAISE_ERRNO(kBufferOverflowError, "vsnprintf() failed: value too large");
  }

  return pos;
}

std::unique_ptr<FileOutputStream> FileOutputStream::openFile(
    const std::string& file_path,
    int flags /* = O_CREAT | O_TRUNC */,
    int permissions /* = 0666 */) {
  int fd = -1;
  auto fp = file_path.c_str();

  flags |= O_WRONLY;

  if ((flags & O_CREAT) == O_CREAT) {
    fd = open(fp, flags, permissions);
  } else {
    fd = open(fp, flags);
  }

  if (fd < 1) {
    RAISE_ERRNO(kIOError, "error opening file '%s'", fp);
  }

  return std::unique_ptr<FileOutputStream>(new FileOutputStream(fd, true));
}

FileOutputStream::FileOutputStream(
    int fd,
    bool close_on_destroy /* = false */) :
    fd_(fd),
    close_on_destroy_(close_on_destroy) {}

FileOutputStream::~FileOutputStream() {
  if (fd_ >= 0 && close_on_destroy_) {
    close(fd_);
  }
}

size_t FileOutputStream::write(const char* data, size_t size) {
  int bytes_written = -1;

  bytes_written = ::write(fd_, data, size);

  if (bytes_written < 0) {
    RAISE_ERRNO(kIOError, "write() failed");
  }

  return bytes_written;
}

size_t FileOutputStream::printf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int pos = vdprintf(fd_, format, args);
  va_end(args);

  if (pos < 0) {
    RAISE_ERRNO(kIOError, "vdprintf() failed");
  }

  return pos;
}

std::unique_ptr<StringOutputStream> StringOutputStream::fromString(
    std::string* string) {
  return std::unique_ptr<StringOutputStream>(new StringOutputStream(string));
}

StringOutputStream::StringOutputStream(std::string* string) : str_(string) {}

size_t StringOutputStream::write(const char* data, size_t size) {
  *str_ += std::string(data, size);
  return size;
}

std::unique_ptr<BufferOutputStream> BufferOutputStream::fromBuffer(
    Buffer* buf) {
  return std::unique_ptr<BufferOutputStream>(new BufferOutputStream(buf));
}

BufferOutputStream::BufferOutputStream(Buffer* buf) : buf_(buf) {}

size_t BufferOutputStream::write(const char* data, size_t size) {
  buf_->append(data, size);
  return size;
}

} // fnord

