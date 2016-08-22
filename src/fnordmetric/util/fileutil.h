/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_FILE_H_
#define _FNORDMETRIC_UTIL_FILE_H_
#include <functional>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include "buffer.h"

namespace fnordmetric {

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
   * Truncate a file
   */
  static void truncate(const std::string& filename, size_t size);

  /**
   * Read a file
   */
  static Buffer read(
      const std::string& filename,
      size_t offset = 0,
      size_t limit = 0);

};

}
#endif
