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
#ifndef _clip_OUTPUTSTREAM_H
#define _clip_OUTPUTSTREAM_H
#include <fcntl.h>
#include <memory>
#include <mutex>
#include "buffer.h"

namespace clip {

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
  virtual size_t write(const char* data, size_t size) = 0;
  virtual size_t write(const std::string& data);
  virtual size_t write(const Buffer& buf);
  virtual size_t printf(const char* format, ...);

  mutable std::mutex mutex_;
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
  size_t write(const char* data, size_t size) override;

  size_t printf(const char* format, ...) override;

protected:
  int fd_;
  bool close_on_destroy_;
};

class StringOutputStream : public OutputStream {
public:

  /**
   * Create a new OutputStream from the provided string
   *
   * @param string the input string
   */
  static std::unique_ptr<StringOutputStream> fromString(std::string* string);

  /**
   * Create a new OutputStream from the provided string
   *
   * @param string the input string
   */
  StringOutputStream(std::string* string);

  /**
   * Write the next n bytes to the file. This may raise an exception.
   * Returns the number of bytes that have been written.
   *
   * @param data a pointer to the data to be written
   * @param size then number of bytes to be written
   */
  size_t write(const char* data, size_t size) override;

protected:
  std::string* str_;
};

class BufferOutputStream : public OutputStream {
public:

  /**
   * Create a new OutputStream from the provided string
   *
   * @param buf the output buffer
   */
  static std::unique_ptr<BufferOutputStream> fromBuffer(Buffer* buf);

  /**
   * Create a new OutputStream from the provided buffer
   *
   * @param buf the output buffer
   */
  BufferOutputStream(Buffer* string);

  /**
   * Write the next n bytes to the file. This may raise an exception.
   * Returns the number of bytes that have been written.
   *
   * @param data a pointer to the data to be written
   * @param size then number of bytes to be written
   */
  size_t write(const char* data, size_t size) override;

protected:
  Buffer* buf_;
};

}
#endif
