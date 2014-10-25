/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_IO_FILE_H_
#define _FNORD_IO_FILE_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace io {

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

  void seekTo(int pos);
  size_t read(void* buf, size_t buf_len);

  int fd() const;
  size_t size() const;

protected:

  File(int fd);

  int fd_;
};

}
}
#endif
