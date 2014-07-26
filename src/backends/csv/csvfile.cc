/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include "csvfile.h"
#include "../../util/runtimeexception.h"

namespace fnordmetric {
namespace csv_backend {

std::unique_ptr<CSVFile> CSVFile::openFile(
    const std::string& file_path,
    char column_seperator /* = ',' */,
    char row_seperator /* = '\n' */,
    char quote_char /* = '"' */) {
  auto fp = file_path.c_str();
  int fd = open(fp, O_RDONLY);

  if (fd < 1) {
    throw RUNTIME_EXCEPTION_ERRNO(
        &typeid(CSVFile),
        ERR_CSV_CANNOT_OPEN_FILE,
        "error opening file '%s'",
        fp);
  }

  auto csv_file = new CSVFile(fd);
  return std::unique_ptr<CSVFile>(csv_file);
}

CSVFile::CSVFile(
    int fd,
    char column_seperator /* = ',' */,
    char row_seperator /* = '\n' */,
    char quote_char /* = '"' */) :
    fd_(fd),
    buf_len_(0),
    buf_pos_(0) {
  assert(fd > 0);
}

void CSVFile::readNextRow(std::vector<std::string>* target) {
  target->emplace_back(readNextColumn());
}

std::string CSVFile::readNextColumn() {
  if (buf_pos_ < buf_len_) {
    readNextChunk();
  }
}

bool CSVFile::readNextChunk() {
  int bytes_read = read(fd_, buf_, sizeof(buf_));

  if (bytes_read < 0) {
    throw RUNTIME_EXCEPTION_ERRNO(
        &typeid(CSVFile),
        ERR_CSV_READ_ERROR,
        "read() failed");
  }

  return bytes_read == 0;
}

}
}
