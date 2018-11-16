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
#ifndef _signaltk_UTIL_FILEUTIL_H_
#define _signaltk_UTIL_FILEUTIL_H_
#include "buffer.h"
#include "stdtypes.h"

namespace signaltk {

class FileUtil {
public:

  /**
   * Create a new directory
   */
  static void mkdir(const std::string& dirname);

  /**
   * Create one or more directories recursively
   */
  static void mkdir_p(const std::string& dirname);

  /**
   * Check if a file exists
   */
  static bool exists(const std::string& dirname);

  /**
   * Check if a file exists and is a directory
   */
  static bool isDirectory(const std::string& dirname);

  /**
   * Return the size of the file
   */
  static size_t size(const std::string& filename);

  /**
   * Join two paths
   */
  static std::string joinPaths(const std::string& p1, const std::string p2);

  /**
   * List files in a directory
   */
  static void ls(
      const std::string& dirname,
      std::function<bool(const std::string&)> callback);

  /**
   * Delete a file
   */
  static void rm(const std::string& filename);

  /**
   * Move a file
   */
  static void mv(const std::string& src, const std::string& dst);

  /**
   * Truncate a file
   */
  static void truncate(const std::string& filename, size_t size);

  /**
   * Read a while file
   */
  static Buffer read(const std::string& filename);

  /**
   * Write a while file
   */
  static void write(const std::string& filename, const Buffer& data);

  /**
   * Copy a file
   */
  static void cp(const std::string& src, const std::string& destination);

  /**
   * Return the size of a directory (like du -c)
   */
  static size_t du_c(const std::string& path);

};

}
#endif
