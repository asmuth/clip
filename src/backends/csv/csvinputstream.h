/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CSVINPUTSTREAM_H
#define _FNORDMETRIC_CSVINPUTSTREAM_H
#include <string>
#include <vector>
#include <memory>
#include "../../util/inputstream.h"
#include "../../util/unittest.h"

namespace fnordmetric {
namespace csv_backend {

class CSVInputStream {
public:

  /**
   * Open a new csv file input stream from the provided file path. Throws an
   * exception if the file cannot be opened.
   *
   * @param file_path the path to the csv file
   */
  static std::unique_ptr<CSVInputStream> openFile(
      const std::string& file_path,
      char column_seperator = ',',
      char row_seperator = '\n',
      char quote_char = '"');

  /**
   * Create a new CSVInputStream from the provided InputStream.
   */
  explicit CSVInputStream(
      std::unique_ptr<fnordmetric::util::InputStream>&& input_stream,
      char column_seperator = ',',
      char row_seperator = '\n',
      char quote_char = '"');

  /**
   * Read the next row from the csv file
   */
  void readNextRow(std::vector<std::string>* target);

protected:

  /**
   * Read the next column from the csv file
   */
  std::string readNextColumn();

  std::unique_ptr<fnordmetric::util::InputStream> input_;
  const char column_seperator_;
  const char row_seperator_;
  const char quote_char_;
};

}
}
#endif
