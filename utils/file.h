/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FNORD_IO_FILE_H_
#define _FNORD_IO_FILE_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace signaltk {
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
