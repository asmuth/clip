/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sstable/binaryformat.h>
#include <fnordmetric/sstable/sstablereader.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnord {
namespace sstable {

SSTableReader::SSTableReader(io::File&& file) : file_(std::move(file)) {
  BinaryFormat::FileHeader header;

  auto file_size = file_.size();
  if (file_size < sizeof(header)) {
    RAISE(kIllegalStateError, "not a valid sstable");
  }

  file_.seekTo(0);
  file_.read(&header, sizeof(header));

  if (header.magic != BinaryFormat::kMagicBytes) {
    RAISE(kIllegalStateError, "not a valid sstable");
  }

  body_size_ = header.body_size;
  header_size_ = header.header_size;

  if (sizeof(header) + header_size_ + body_size_ > file_size) {
    if (header.magic != BinaryFormat::kMagicBytes) {
      RAISE(kIllegalStateError, "file metadata offsets exceed file bounds");
    }
  }
}

util::Buffer SSTableReader::readHeader() {
  file_.seekTo(sizeof(BinaryFormat::FileHeader));
  util::Buffer buf(header_size_);
  file_.read(&buf);
  return buf;
}

size_t SSTableReader::bodySize() const {
  return body_size_;
}

size_t SSTableReader::headerSize() const {
  return header_size_;
}


}
}

