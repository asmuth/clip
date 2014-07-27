/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_OUTPUTSTREAM_H
#define _FNORDMETRIC_OUTPUTSTREAM_H
#include <memory>

namespace fnordmetric {
namespace util {

class OutputStream {
public:
  OutputStream() {}
  OutputStream(const OutputStream& other) = delete;
  OutputStream& operator=(const OutputStream& other) = delete;
  virtual ~OutputStream() {}

  /**
   * Get the stdout output stream
   */
  static std::unique_ptr<OutputStream> getStdout();

  /**
   * Get the stderr output stream
   */
  static std::unique_ptr<OutputStream> getStderr();

  /**
   * Write the next n bytes to the output stream. This may raise an exception.
   *
   * @param data a pointer to the data to be written
   * @param size then number of bytes to be written
   */
  virtual bool write(char* data, size_t size) = 0;

};

class FileOutputStream : public OutputStream {

  /**
   * Create a new FileOuputStream instance from the provided filedescriptor. If 
   * close on_destroy is true, the fd will be close()ed when the output stream
   * is destroyed.
   *
   * @param fd a valid fd
   * @param close_on_destroy close the fd on destroy?
   */
  explicit FileOutputStream(int fd, bool close_on_destroy = false);

  /**
   * Write the next n bytes to the file. This may raise an exception.
   *
   * @param data a pointer to the data to be written
   * @param size then number of bytes to be written
   */
  virtual bool write(char* data, size_t size) = 0;

};

}
}
#endif
