/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_CSVINPUTSTREAM_H
#define _FNORDMETRIC_CSVINPUTSTREAM_H
#include <string>
#include <vector>
#include <memory>
#include "fnord-base/io/inputstream.h"

namespace fnordmetric {
namespace query {
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
      std::unique_ptr<fnord::RewindableInputStream>&& input_stream,
      char column_seperator = ',',
      char row_seperator = '\n',
      char quote_char = '"');

  /**
   * Read the next row from the csv file. Returns true if a row was read and
   * false on EOF. May raise an exception.
   */
  bool readNextRow(std::vector<std::string>* target);

  /**
   * Skip the next row from the csv file. Returns true if a row was skipped and
   * false on EOF. May raise an exception.
   */
  bool skipNextRow();

  /**
   * Rewind the input stream
   */
  void rewind();

  /**
   * Return the input stream
   */
  const fnord::RewindableInputStream& getInputStream() const;

protected:

  /**
   * Read the next column from the csv file
   */
  std::string readNextColumn();

  std::unique_ptr<fnord::RewindableInputStream> input_;
  const char column_seperator_;
  const char row_seperator_;
  const char quote_char_;
};

}
}
}
#endif
