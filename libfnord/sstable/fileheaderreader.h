/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_FILEHEADERREADER_H
#define _FNORDMETRIC_METRICDB_FILEHEADERREADER_H
#include <fnord/util/binarymessagereader.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace sstable {

class FileHeaderReader : public fnord::util::BinaryMessageReader {
public:

  FileHeaderReader(
      void* buf,
      size_t buf_size);

  /**
   * Verify the checksums and boundaries. Returns true if valid, false otherwise
   */
  bool verify();

  /**
   * Returns the size of the header, including userdata in bytes
   */
  size_t headerSize() const;

  /**
   * Returns the body size in bytes
   */
  size_t bodySize() const;

  /**
   * Returns the header userdata size in bytes
   */
  size_t userdataSize() const;

  /**
   * Return the userdata
   */
  void readUserdata(const void** userdata, size_t* userdata_size);

protected:
  uint64_t body_size_;
  uint32_t userdata_checksum_;
  uint32_t userdata_size_;
  size_t userdata_offset_;
};

}
}

#endif
