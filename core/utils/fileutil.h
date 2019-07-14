/**
 * This file is part of the "fviz" project
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
#ifndef _fviz_UTIL_FILEUTIL_H_
#define _fviz_UTIL_FILEUTIL_H_
#include "buffer.h"
#include "stdtypes.h"
#include "return_code.h"

namespace fviz {

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

ReturnCode read_file(const std::string& path, std::string* data);

}
#endif
