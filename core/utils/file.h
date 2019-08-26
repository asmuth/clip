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
#ifndef _FNORD_IO_FILE_H_
#define _FNORD_IO_FILE_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace clip {
class Buffer;

#undef O_APPEND
class File {
public:

  enum kOpenFlags {
    /**
     * Open in read/write mode. At least one must be set
     */
    O_READ = 1,
    O_WRITE = 2,

    /**
     * Create the file if it doesn't exist and error if it exists
     */
    O_CREATE = 4,

    /**
     * Create the file if it doesn't exist and open it if it exists
     */
    O_CREATEOROPEN = 8,

    /**
     * Append to the file
     */
    O_APPEND = 16,

    /*
     * Truncate the file when opening it
     */
    O_TRUNCATE = 32,

    /**
     * Unlink the file after opening
     */
    O_AUTODELETE = 64,

    /**
     * Do not set the close on exec flag
     */
    O_ALLOWFORK = 128
  };

  /**
   * Open a file
   *
   * @param filename the filename/path
   * @param flags flags from kOpenFlags
   */
  static File openFile(
      const std::string& filename,
      int flags);


  File(File&& move);
  File(const File& copy) = delete;
  ~File();

  File& operator=(const File& copy) = delete;

  void seekTo(size_t pos);
  size_t read(void* buf, size_t buf_len);
  size_t read(Buffer* buf);

  void write(const Buffer& buf);
  void write(const void* buf, size_t buf_len);

  int fd() const;
  size_t size() const;

  File clone() const;
  void truncate(size_t new_size);

  bool isWritable() const;

protected:

  File(int fd, int flags);
  int fd_;
  int flags_;
};

}
#endif
