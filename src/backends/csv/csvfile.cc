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
  int fd = open(file_path.c_str(), O_RDONLY);

  if (fd < 1) {
    throw RUNTIME_EXCEPTION(
      "error opening file '%s':",
      file_path.c_str());
  }

  auto csv_file = new CSVFile(fd);
  return std::unique_ptr<CSVFile>(csv_file);
}

CSVFile::CSVFile(int fd) : fd_(fd) {
  assert(fd > 0);
}

}
}
