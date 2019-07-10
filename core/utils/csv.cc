/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include "csv.h"

namespace plotfx {

ReturnCode parseCSV(
    std::string input,
    const CSVParserConfig& opts,
    std::list<std::vector<std::string>>* output) {
  input.push_back(0);

  std::vector<std::string> row;
  std::string buffer;
  bool quoted = false;
  bool escaped = false;
  size_t row_index = 0;

  for (const auto& byte : input) {
    if (byte == opts.escape_char) {
      if (escaped) {
        buffer += opts.escape_char;
        escaped = false;
      } else {
        escaped = true;
      }
      continue;
    }

    if (!escaped && byte == opts.quote_char) {
      quoted = !quoted;
      continue;
    }

    if ((!quoted && byte == opts.column_separator) ||
        (!quoted && byte == opts.line_separator) ||
        (!quoted && byte == 0)) {
      row.emplace_back(buffer);
      buffer.clear();

      if (byte == opts.line_separator) {
        ++row_index;
        output->push_back(row);
        row.clear();
      }

      continue;
    }

    buffer += byte;
    escaped = false;
  }

  if (quoted || buffer.size() > 0) {
    return ReturnCode::error("EIO", "invalid csv line");
  }

  return ReturnCode::success();
}

} // namespace plotfx


