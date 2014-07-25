/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CSVFILE_H
#define _FNORDMETRIC_CSVFILE_H
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace csv_backend {

class CSVFile {
  friend class CSVBackendTest;
public:

  enum ErrorCodes {
    ERR_CSV_CANNOT_OPEN_FILE = 4000
  };

  /**
   * Open a new csv file from the provided file path. Throws an exception if
   * the file cannot be opened.
   *
   * @param file_path the path to the csv file
   */
  static std::unique_ptr<CSVFile> openFile(const std::string& file_path);

  CSVFile(const CSVFile& other) = delete;
  CSVFile& operator=(const CSVFile& other) = delete;

  /**
   * Create a new CSVFile instance with the provided filedescriptor. The csv
   * file takes ownership of the fd and will close() it when destructed.
   *
   * @param fd a valid an opened fd, transfers ownership and closes on destruct
   */
  explicit CSVFile(int fd);

protected:
  int fd_;
};

}
}
#endif
