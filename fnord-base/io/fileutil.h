/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_FILEUTIL_H_
#define _FNORDMETRIC_UTIL_FILEUTIL_H_
#include "fnord-base/buffer.h"
#include "fnord-base/stdtypes.h"

namespace fnord {

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
