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
#include <cstdarg>
#include <fcntl.h>
#include <memory>
#include <stdarg.h>
#include <string>
#include <unistd.h>
#include "metricd/util/buffer.h"
#include "metricd/util/exception.h"
#include "metricd/util/outputstream.h"
#include "metricd/util/ieee754.h"

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

void OutputStream::appendUInt8(uint8_t value) {
  write((char*) &value, sizeof(value));
}

void OutputStream::appendUInt16(uint16_t value) {
  write((char*) &value, sizeof(value));
}

void OutputStream::appendUInt32(uint32_t value) {
  write((char*) &value, sizeof(value));
}

void OutputStream::appendUInt64(uint64_t value) {
  write((char*) &value, sizeof(value));
}

void OutputStream::appendDouble(double value) {
  auto bytes = IEEE754::toBytes(value);
  write((char*) &bytes, sizeof(bytes));
}

void OutputStream::appendString(const std::string& string) {
  write(string.data(), string.size());
}

void OutputStream::appendLenencString(const std::string& string) {
  appendLenencString(string.data(), string.size());
}

void OutputStream::appendLenencString(const void* data, size_t size) {
  appendVarUInt(size);
  write((char*) data, size);
}

void OutputStream::appendVarUInt(uint64_t value) {
  unsigned char buf[10];
  size_t bytes = 0;
  do {
    buf[bytes] = value & 0x7fU;
    if (value >>= 7) buf[bytes] |= 0x80U;
    ++bytes;
  } while (value);

  write((char*) buf, bytes);
}

bool OutputStream::isTTY() const {
  return false;
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

std::unique_ptr<FileOutputStream> FileOutputStream::fromFileDescriptor(
    int fd,
    bool close_on_destroy /* = false */) {
  std::unique_ptr<FileOutputStream> stream(
      new FileOutputStream(fd, close_on_destroy));

  return stream;
}

std::unique_ptr<FileOutputStream> FileOutputStream::fromFile(File&& file) {
  std::unique_ptr<FileOutputStream> stream(new FileOutputStream(std::move(file)));
  return stream;
}

FileOutputStream::FileOutputStream(
    int fd,
    bool close_on_destroy /* = false */) :
    fd_(fd),
    close_on_destroy_(close_on_destroy) {}

FileOutputStream::FileOutputStream(
    File&& file) :
    FileOutputStream(file.releaseFD(), true) {}

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

bool FileOutputStream::isTTY() const {
  return ::isatty(fd_);
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

