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
#pragma once
#include <string>
#include <vector>
#include "return_code.h"

namespace fviz {

struct CSVParserConfig {
  CSVParserConfig() :
      line_separator('\n'),
      column_separator(','),
      quote_char('\"'),
      escape_char('\\') {}

  char line_separator;
  char column_separator;
  char quote_char;
  char escape_char;
};

using CSVData = std::list<std::vector<std::string>>;

ReturnCode parseCSV(
    std::string input,
    const CSVParserConfig& config,
    CSVData* output);

} // namespace fviz

