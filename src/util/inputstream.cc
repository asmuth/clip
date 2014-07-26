/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include "inputstream.h"
namespace fnordmetric {
namespace util {

std::unique_ptr<InputStream> InputStream::openFile(
    const std::string& file_path,
    char column_seperator /* = ',' */,
    char row_seperator /* = '\n' */,
    char quote_char /* = '"' */) {
  auto fp = file_path.c_str();
  int fd = open(fp, O_RDONLY);

  if (fd < 1) {
    throw RUNTIME_EXCEPTION_ERRNO(
        &typeid(InputStream),
        ERR_CSV_CANNOT_OPEN_FILE,
        "error opening file '%s'",
        fp);
  }

  auto csv_file = new InputStream(fd);
  return std::unique_ptr<InputStream>(csv_file);
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
