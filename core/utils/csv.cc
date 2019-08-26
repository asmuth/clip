/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "csv.h"
#include <iostream>

namespace clip {

ReturnCode csv_parse_field_escaped(
    std::string input,
    size_t* offset,
    std::string* field) {
  if (input[(*offset)++] != '"') {
    return error(
        ERROR,
        "RFC4180: quoted strings must start with a double quote (\") character");
  }

  while (*offset != input.size()) {
    if (*offset + 1 != input.size() &&
        input[*offset] == '"' &&
        input[*offset + 1] == '"') {
      // escaped quote char
      field->push_back('"');
      *offset += 2;
      continue;
    } else if (input[*offset] == '"') {
      // string terminator
      *offset += 1;
      return OK;
    } else {
      // consume character
      field->push_back(input[(*offset)++]);
      continue;
    }
  }

  return errorf(ERROR, "unterminated string: '{}'", *field);
}

ReturnCode csv_parse_field_literal(
    std::string input,
    size_t* offset,
    std::string* field) {
  while (*offset != input.size()) {
    switch (input[*offset]) {
      // string terminator
      case '\r':
      case '\n':
      case ',':
        return OK;
      // literal char
      default:
        field->push_back(input[(*offset)++]);
        continue;
    }
  }

  return OK;
}

ReturnCode csv_parse_field(
    std::string input,
    size_t* offset,
    std::string* field) {
  if (input[*offset] == '"') {
    return csv_parse_field_escaped(input, offset, field);
  } else {
    return csv_parse_field_literal(input, offset, field);
  }

  return OK;
}

ReturnCode csv_parse_record(
    std::string input,
    size_t* offset,
    std::vector<std::string>* record) {
  while (*offset != input.size()) {
    // parse next field
    std::string field;
    if (auto rc = csv_parse_field(input, offset, &field); rc) {
      record->push_back(field);
    } else {
      return rc;
    }

    // end of file
    if (*offset == input.size()) {
      break;
    }

    // field terminator
    switch (input[*offset]) {
      // newline
      case '\r':
      case '\n':
        return OK;
      // next field
      case ',':
        ++(*offset);
        continue;
      // invalid seperator
      default:
        return error(
            ERROR,
            "RFC4180: fields must be separated by a comma (,) character");
    }
  }

  return OK;
}

ReturnCode csv_parse(std::string input, CSVData* output) {
  for (size_t offset = 0; offset != input.size(); ) {
    // next record
    std::vector<std::string> record;
    if (auto rc = csv_parse_record(input, &offset, &record)) {
      output->push_back(record);
    } else {
      return rc;
    }

    // end of file
    if (offset == input.size()) {
      break;
    }

    // end of line (CRLF)
    switch (input[offset]) {
      case '\r':
        if (offset + 1 == input.size() || input[++offset] != '\n') {
          return error(
              ERROR,
              "RFC4180: the carriage return ('\\r') character must be followed "
              "by a newline ('\\n') character unless escaped");
        } else {
          /* fallthrough */
        }
      case '\n':
        ++offset;
        break;
    }
  }

  return OK;
}

} // namespace clip

