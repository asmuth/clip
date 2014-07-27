/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace util {

std::unique_ptr<InputStream> InputStream::getStdin() {
  auto stdin_stream = new FileInputStream(0, false);
  return std::unique_ptr<InputStream>(stdin_stream);
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
    RAISE_ERRNO(RuntimeException, "error opening file '%s'", fp);
  }

  auto csv_file = new FileInputStream(fd);
  csv_file->readNextChunk();
  return std::unique_ptr<FileInputStream>(csv_file);
}

FileInputStream::FileInputStream(
    int fd,
    bool close_on_destroy /* = false */) :
    fd_(fd),
    close_on_destroy_(close_on_destroy) {}

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
    RAISE_ERRNO(RuntimeException, "read() failed");
  }

  buf_pos_ = 0;
  buf_len_ = bytes_read;
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

}
}
