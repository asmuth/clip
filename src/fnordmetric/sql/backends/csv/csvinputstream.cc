/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include "csvinputstream.h"
#include "fnord-base/exception.h"
#include "fnord-base/io/inputstream.h"

namespace fnordmetric {
namespace query {
namespace csv_backend {

std::unique_ptr<CSVInputStream> CSVInputStream::openFile(
    const std::string& file_path,
    char column_seperator /* = ',' */,
    char row_seperator /* = '\n' */,
    char quote_char /* = '"' */) {
  auto file = fnord::FileInputStream::openFile(file_path);
  file->readByteOrderMark();

  auto csv_file = new CSVInputStream(
      std::move(file),
      column_seperator,
      row_seperator,
      quote_char);

  return std::unique_ptr<CSVInputStream>(csv_file);
}

CSVInputStream::CSVInputStream(
    std::unique_ptr<fnord::RewindableInputStream>&& input_stream,
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

bool CSVInputStream::skipNextRow() {
  std::vector<std::string> devnull;
  return readNextRow(&devnull);
}

void CSVInputStream::rewind() {
  input_->rewind();
}

const fnord::RewindableInputStream& CSVInputStream::getInputStream()
    const {
  return *input_;
}

}
}
}
