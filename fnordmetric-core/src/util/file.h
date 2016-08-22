/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
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
#ifndef _STX_IO_FILE_H_
#define _STX_IO_FILE_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <eventql/util/stdtypes.h>

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
      int flags,
      int permissions = 0666);


  File(File&& other);
  File(const File& other) = delete;
  ~File();

  File& operator=(const File& other) = delete;
  File& operator=(File&& other);

  void seekTo(size_t pos);
  size_t read(void* buf, size_t buf_len);
  size_t read(Buffer* buf);

  void write(const Buffer& buf);
  void write(const String& buf);
  void write(const void* buf, size_t buf_len);

  void pwrite(uint64_t position, const Buffer& buf);
  void pwrite(uint64_t position, const String& buf);
  void pwrite(uint64_t position, const void* buf, size_t buf_len);

  int fd() const;
  size_t size() const;

  File clone() const;
  void truncate(size_t new_size);
  void fsync(bool sync_metadata = false);

  bool isWritable() const;

  int releaseFD();

protected:

  File(int fd, int flags);
  int fd_;
  int flags_;
};

#endif
