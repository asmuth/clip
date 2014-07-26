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
#include "../../../util/inputstream.h"
#include "../../../util/runtimeexception.h"

namespace fnordmetric {
namespace query {
namespace csv_backend {

std::unique_ptr<CSVInputStream> CSVInputStream::openFile(
    const std::string& file_path,
    char column_seperator /* = ',' */,
    char row_seperator /* = '\n' */,
    char quote_char /* = '"' */) {
  auto file = fnordmetric::util::FileInputStream::openFile(file_path);
  file->readByteOrderMark();

  auto csv_file = new CSVInputStream(
      std::move(file),
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

// FIXPAUL quotechar escaping...
bool CSVInputStream::readNextRow(std::vector<std::string>* target) {
  bool eof = false;

  for (;;) {
    std::string column;
    char byte;
    bool quoted = false;

    for (;;) {
      if (!input_->readNextByte(&byte)) {
        eof = true;
        break;
      }

      if (!quoted && byte == column_seperator_) {
        break;
      }

      if (!quoted && byte == row_seperator_) {
        break;
      }

      if (byte == quote_char_) {
        quoted = !quoted;
      }

      column += byte;
    }

    target->emplace_back(column);

    if (eof || byte == row_seperator_) {
      break;
    }
  }

  return !eof;
}

}
}
}
