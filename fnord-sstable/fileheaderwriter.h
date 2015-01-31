/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_FILEHEADERWRITER_H
#define _FNORDMETRIC_METRICDB_FILEHEADERWRITER_H
#include <fnord-base/util/binarymessagewriter.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace sstable {

class FileHeaderWriter : public fnord::util::BinaryMessageWriter {
public:
  static size_t calculateSize(size_t userdata_size);

  /**
   * Write a new file header
   */
  FileHeaderWriter(
      void* buf,
      size_t buf_size,
      size_t body_size,
      const void* userdate,
      size_t userdata_size);

  /**
   * Open an existing file header for update
   */
  FileHeaderWriter(
      void* buf,
      size_t buf_size);

  /**
   * Update/rewrite the body size field
   */
  void updateBodySize(size_t body_size);

};

}
}

#endif
