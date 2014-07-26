/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <string>
#include <fcntl.h>
#include "inputstream.h"
#include "runtimeexception.h"

namespace fnordmetric {
namespace util {

std::unique_ptr<FileInputStream> FileInputStream::openFile(
    const std::string& file_path) {
  auto fp = file_path.c_str();
  int fd = open(fp, O_RDONLY);

  if (fd < 1) {
    RAISE_ERRNO(RuntimeException, "error opening file '%s'", fp);
  }

  auto csv_file = new FileInputStream(fd);
  return std::unique_ptr<FileInputStream>(csv_file);
}

FileInputStream::FileInputStream(int fd) : fd_(fd) {}

bool FileInputStream::readNextByte(char* target) {

}

/*
bool CSVInputStream::readNextChunk() {
  int bytes_read = read(fd_, buf_, sizeof(buf_));

  if (bytes_read < 0) {
    throw RUNTIME_EXCEPTION_ERRNO(
        &typeid(CSVInputStream),
        ERR_CSV_READ_ERROR,
        "read() failed");
  }

  return bytes_read == 0;
}
*/

}
}
