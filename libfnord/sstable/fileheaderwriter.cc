/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/sstable/binaryformat.h>
#include <fnord/sstable/fileheaderwriter.h>
#include <fnord/hash/fnv.h>

namespace fnord {
namespace sstable {

size_t FileHeaderWriter::calculateSize(size_t userdata_size) {
  return 22 + userdata_size; // FIXPAUL
}

FileHeaderWriter::FileHeaderWriter(
    void* buf,
    size_t buf_size,
    size_t body_size,
    const void* userdata,
    size_t userdata_size) :
    fnord::util::BinaryMessageWriter(buf, buf_size) {
  appendUInt32(BinaryFormat::kMagicBytes);
  appendUInt16(BinaryFormat::kVersion);
  appendUInt64(body_size);

  if (userdata_size > 0) {
    hash::FNV<uint32_t> fnv;
    auto userdata_checksum = fnv.hash(userdata, userdata_size);
    appendUInt32(userdata_checksum);
    appendUInt32(userdata_size);
    append(userdata, userdata_size);
  } else {
    appendUInt32(0);
    appendUInt32(0);
  }
}

FileHeaderWriter::FileHeaderWriter(
    void* buf,
    size_t buf_size) :
    fnord::util::BinaryMessageWriter(buf, buf_size) {}

void FileHeaderWriter::updateBodySize(size_t body_size) {
  updateUInt64(6, body_size); // FIXPAUL
}


}
}

