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
      fnordmetric::util::FileInputStream::openFile(file_path),
      column_seperator,
      row_seperator,
      quote_char);

  return std::unique_ptr<CSVInputStream>(csv_file);
}

CSVInputStream::CSVInputStream(
    std::unique_ptr<fnordmetric::util::InputStream>&& input_stream,
    char column_seperator /* = ',' */,
    char row_seperator /* = '\n' */,
    char quote_char /* = '"' */) :
    input_(std::move(input_stream)),
    column_seperator_(column_seperator),
    row_seperator_(row_seperator),
    quote_char_(quote_char) {}

void CSVInputStream::readNextRow(std::vector<std::string>* target) {
  for (;;) {
    std::string column;
    char byte;

    while (input_->readNextByte(&byte)) {
      if (byte == column_seperator_) {
        break;
      }

      if (byte == row_seperator_) {
        break;
      }

      column += byte;
    }

    target->emplace_back(column);

    if (byte == row_seperator_) {
      break;
    }
  }
}

}
}
