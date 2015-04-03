/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <fnord-base/buffer.h>
#include <fnord-base/exception.h>
#include <fnord-base/io/inputstream.h>

namespace fnord {

std::unique_ptr<InputStream> InputStream::getStdin() {
  auto stdin_stream = new FileInputStream(0, false);
  return std::unique_ptr<InputStream>(stdin_stream);
}

InputStream::InputStream(
    const std::string& filename /* = "<anonymous input stream>" */) :
    filename_(filename) {}

const std::string& InputStream::getFileName() const {
  return filename_;
}

void InputStream::setFileName(const std::string& filename) {
  filename_ = filename;
}

// FIXPAUL: optimize?
size_t InputStream::readNextBytes(std::string* target, size_t n_bytes) {
  char byte;
  size_t length;

  for (length = 0; length < n_bytes && readNextByte(&byte); ++length) {
    *target += byte;
  }

  return length;
}

// FIXPAUL: optimize?
size_t InputStream::readNextBytes(Buffer* target, size_t n_bytes) {
  char byte;
  size_t length;

  target->reserve(n_bytes);

  for (length = 0; length < n_bytes && readNextByte(&byte); ++length) {
    target->append(&byte, sizeof(byte));
  }

  return length;
}

// FIXPAUL: optimize?
size_t InputStream::readUntilEOF(std::string* target) {
  char byte;
  size_t length;

  for (length = 0; readNextByte(&byte); ++length) {
    *target += byte;
  }

  return length;
}

std::unique_ptr<FileInputStream> FileInputStream::openFile(
    const std::string& file_path) {
  auto fp = file_path.c_str();
  int fd = open(fp, O_RDONLY);

  if (fd < 1) {
    RAISE_ERRNO(kIOError, "error opening file '%s'", fp);
  }

  auto file = new FileInputStream(fd, true);
  file->readNextChunk();
  file->setFileName(file_path);
  return std::unique_ptr<FileInputStream>(file);
}

FileInputStream::FileInputStream(
    int fd,
    bool close_on_destroy /* = false */) :
    fd_(fd),
    close_on_destroy_(close_on_destroy),
    buf_len_(0),
    buf_pos_(0) {}

FileInputStream::~FileInputStream() {
  if (fd_ >= 0 && close_on_destroy_) {
    close(fd_);
  }
}

bool FileInputStream::readNextByte(char* target) {
  if (buf_pos_ >= buf_len_) {
    readNextChunk();
  }

  if (buf_pos_ < buf_len_) {
    *target = buf_[buf_pos_++];
    return true;
  } else {
    return false;
  }
}

FileInputStream::kByteOrderMark FileInputStream::readByteOrderMark() {
  static char kByteOrderMarkUTF8[] = "\xEF\xBB\xBF";
  if (buf_pos_ + 2 < buf_len_ &&
      strncmp(buf_ + buf_pos_, kByteOrderMarkUTF8, 3) == 0) {
    buf_pos_ += 3;
    return BOM_UTF8;
  }

  return BOM_UNKNOWN;
}

void FileInputStream::readNextChunk() {
  int bytes_read = read(fd_, buf_, sizeof(buf_));

  if (bytes_read < 0) {
    RAISE_ERRNO(kIOError, "read(%s) failed", getFileName().c_str());
  }

  buf_pos_ = 0;
  buf_len_ = bytes_read;
}

void FileInputStream::rewind() {
  buf_pos_ = 0;
  buf_len_ = 0;

  if (lseek(fd_, 0, SEEK_SET) < 0) {
    RAISE_ERRNO(kIOError, "lseek(%s) failed", getFileName().c_str());
  }
}

std::unique_ptr<StringInputStream> StringInputStream::fromString(
    const std::string& string) {
  return std::unique_ptr<StringInputStream>(new StringInputStream(string));
}

StringInputStream::StringInputStream(
    const std::string& string) :
    str_(string),
    cur_(0) {
}

bool StringInputStream::readNextByte(char* target) {
  if (cur_ < str_.size()) {
    *target = str_[cur_++];
    return true;
  } else {
    return false;
  }
}

void StringInputStream::rewind() {
  cur_ = 0;
}

std::unique_ptr<BufferInputStream> BufferInputStream::fromBuffer(
    const Buffer* buf) {
  return std::unique_ptr<BufferInputStream>(new BufferInputStream(buf));
}

BufferInputStream::BufferInputStream(
    const Buffer* buf) :
    buf_(buf),
    cur_(0) {
}

bool BufferInputStream::readNextByte(char* target) {
  if (cur_ < buf_->size()) {
    *target = buf_->charAt(cur_++);
    return true;
  } else {
    return false;
  }
}

void BufferInputStream::rewind() {
  cur_ = 0;
}

} // fnord

