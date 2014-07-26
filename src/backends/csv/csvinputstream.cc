/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include "csvinputstream.h"
#include "../../util/inputstream.h"
#include "../../util/runtimeexception.h"

namespace fnordmetric {
namespace csv_backend {

std::unique_ptr<CSVInputStream> CSVInputStream::openFile(
    const std::string& file_path,
    char column_seperator /* = ',' */,
    char row_seperator /* = '\n' */,
    char quote_char /* = '"' */) {
  auto csv_file = new CSVInputStream(
      fnordmetric::util::FileInputStream::openFile(file_path));

  return std::unique_ptr<CSVInputStream>(csv_file);
}

CSVInputStream::CSVInputStream(
    std::unique_ptr<fnordmetric::util::InputStream>&& input_stream,
    char column_seperator /* = ',' */,
    char row_seperator /* = '\n' */,
    char quote_char /* = '"' */) :
    input_(std::move(input_stream)) {}

void CSVInputStream::readNextRow(std::vector<std::string>* target) {
  target->emplace_back(readNextColumn());
}

// FIXPAUL optimize?
std::string CSVInputStream::readNextColumn() {
  std::string column;

  return column;
}

}
}
