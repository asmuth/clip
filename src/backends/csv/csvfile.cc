/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <assert.h>
#include <fcntl.h>
#include "csvfile.h"
#include "../../util/runtimeexception.h"

namespace fnordmetric {
namespace csv_backend {

std::unique_ptr<CSVFile> CSVFile::openFile(const std::string& file_path) {
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

CSVFile::CSVFile(int fd) : fd_(fd) {
  assert(fd > 0);
}

}
}
