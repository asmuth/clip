/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_OUTPUTSTREAM_H
#define _FNORDMETRIC_OUTPUTSTREAM_H
#include <fcntl.h>
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
   * Returns the number of bytes that have been written.
   *
   * @param data a pointer to the data to be written
   * @param size then number of bytes to be written
   */
  virtual size_t write(char* data, size_t size) = 0;

  virtual size_t printf(const char* format, ...) {} // FIXPAUL

};

class FileOutputStream : public OutputStream {
public:

  /**
   * Create a new FileOutputStream instance by opening the provided file for
   * writing. The fille will automatically be close()ed when the output stream
   * is destroyed.
   *
   * @param file_path the path to the file to open
   * @param flags flags to pass to the open() syscall
   */
  static std::unique_ptr<FileOutputStream> openFile(
      const std::string& file_path,
      int flags = O_CREAT | O_TRUNC,
      int permissions = 0666);

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
   * Close the fd if close_on_destroy is true
   */
  ~FileOutputStream();

  /**
   * Write the next n bytes to the file. This may raise an exception.
   * Returns the number of bytes that have been written.
   *
   * @param data a pointer to the data to be written
   * @param size then number of bytes to be written
   */
  size_t write(char* data, size_t size) override;

  size_t printf(const char* format, ...) override;

protected:
  int fd_;
  bool close_on_destroy_;
};

}
}
#endif
