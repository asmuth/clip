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

  file_size_ = file_.size();
  if (file_size_ < sizeof(header)) {
    RAISE(kIllegalStateError, "not a valid sstable");
  }

  file_.seekTo(0);
  file_.read(&header, sizeof(header));

  if (header.magic != BinaryFormat::kMagicBytes) {
    RAISE(kIllegalStateError, "not a valid sstable");
  }

  body_size_ = header.body_size;
  header_size_ = header.header_size;

  if (sizeof(header) + header_size_ + body_size_ > file_size_) {
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

util::Buffer SSTableReader::readFooter(uint32_t type) {
  size_t pos = sizeof(BinaryFormat::FileHeader) + header_size_ + body_size_;

  while (pos < file_size_) {
    BinaryFormat::FooterHeader header;
    file_.seekTo(pos);
    file_.read(&header, sizeof(header));

    if (header.magic != BinaryFormat::kMagicBytes) {
      RAISE(kIllegalStateError, "corrupt sstable footer");
    }

    if (header.type == type) {
      util::Buffer buf(header.footer_size);
      file_.read(&buf);
      return buf;
    }

    pos += sizeof(header) + header.footer_size;
  }

  RAISE(kIndexError, "no such footer found");
  return util::Buffer(0);
}

std::unique_ptr<Cursor> SSTableReader::getCursor() {
  auto cursor = new Cursor(
      file_.clone(),
      sizeof(BinaryFormat::FileHeader) + header_size_,
      sizeof(BinaryFormat::FileHeader) + header_size_ + body_size_);

  return std::unique_ptr<Cursor>(cursor);
}

size_t SSTableReader::bodySize() const {
  return body_size_;
}

size_t SSTableReader::headerSize() const {
  return header_size_;
}

SSTableReader::Cursor::Cursor(
    io::File&& file,
    size_t begin,
    size_t limit) :
    file_(std::move(file)),
    begin_(begin),
    limit_(limit) {
  file_.seekTo(begin);
}

void SSTableReader::Cursor::seekTo(size_t body_offset) {
  if (body_offset >= limit_) {
    RAISE(kIndexError, "body offset exceeds limit");
  }

  seekTo(body_offset);
}

bool SSTableReader::Cursor::next() {
  return false;
}

bool SSTableReader::Cursor::valid() {
  return false;
}

void SSTableReader::Cursor::getKey(void** data, size_t* size) {
}

void SSTableReader::Cursor::getData(void** data, size_t* size) {
}

}
}

