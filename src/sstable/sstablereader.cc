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

SSTableReader::SSTableReader(
    io::File&& file) :
    mmap_(new io::MmappedFile(std::move(file))),
    file_size_(mmap_->size()),
    header_(mmap_->ptr(), file_size_) {
  if (!header_.verify()) {
    RAISE(kIllegalStateError, "corrupt sstable header");
  }

  if (header_.headerSize() + header_.bodySize() > file_size_) {
    RAISE(kIllegalStateError, "file metadata offsets exceed file bounds");
  }
}

void SSTableReader::readHeader(void** userdata, size_t* userdata_size) {
  header_.readUserdata(userdata, userdata_size);
}

util::Buffer SSTableReader::readHeader() {
  void* data;
  size_t size;
  readHeader(&data, &size);

  return util::Buffer(data, size);
}

void SSTableReader::readFooter(
    uint32_t type,
    void** data,
    size_t* size) {
  size_t pos = header_.headerSize() + header_.bodySize();

  while (pos < file_size_) {
    auto footer_header = mmap_->structAt<BinaryFormat::FooterHeader>(pos);
    pos += sizeof(BinaryFormat::FooterHeader);

    if (footer_header->magic != BinaryFormat::kMagicBytes) {
      RAISE(kIllegalStateError, "corrupt sstable footer");
    }

    if (footer_header->type == type) {
      *data = mmap_->structAt<void>(pos);
      *size = footer_header->footer_size;
      return;
    }

    pos += footer_header->footer_size;
  }

  RAISE(kIndexError, "no such footer found");
}

util::Buffer SSTableReader::readFooter(uint32_t type) {
  void* data;
  size_t size;
  readFooter(type, &data, &size);

  return util::Buffer(data, size);
}

std::unique_ptr<Cursor> SSTableReader::getCursor() {
  auto cursor = new Cursor(
      mmap_,
      header_.headerSize(),
      header_.headerSize() + header_.bodySize());

  return std::unique_ptr<Cursor>(cursor);
}

size_t SSTableReader::bodySize() const {
  return header_.bodySize();
}

size_t SSTableReader::headerSize() const {
  return header_.userdataSize();
}

SSTableReader::Cursor::Cursor(
    std::shared_ptr<io::MmappedFile> mmap,
    size_t begin,
    size_t limit) :
    mmap_(std::move(mmap)),
    begin_(begin),
    limit_(limit),
    pos_(begin) {}

void SSTableReader::Cursor::seekTo(size_t body_offset) {
  pos_ = begin_ + body_offset;
}

bool SSTableReader::Cursor::next() {
  auto header = mmap_->structAt<BinaryFormat::RowHeader>(pos_);

  auto next_pos = pos_ += sizeof(BinaryFormat::RowHeader) +
      header->key_size +
      header->data_size;

  if (next_pos < limit_) {
    pos_ = next_pos;
  } else {
    return false;
  }

  return valid();
}

bool SSTableReader::Cursor::valid() {
  auto header = mmap_->structAt<BinaryFormat::RowHeader>(pos_);

  auto row_limit = pos_ + sizeof(BinaryFormat::RowHeader) +
      header->key_size +
      header->data_size;

  return header->key_size > 0 && row_limit <= limit_;
}

void SSTableReader::Cursor::getKey(void** data, size_t* size) {
  if (!valid()) {
    RAISE(kIllegalStateError, "invalid cursor");
  }

  auto header = mmap_->structAt<BinaryFormat::RowHeader>(pos_);
  *data = mmap_->structAt<void>(pos_ + sizeof(BinaryFormat::RowHeader));
  *size = header->key_size;
}

void SSTableReader::Cursor::getData(void** data, size_t* size) {
  if (!valid()) {
    RAISE(kIllegalStateError, "invalid cursor");
  }

  auto header = mmap_->structAt<BinaryFormat::RowHeader>(pos_);
  auto offset = pos_ + sizeof(BinaryFormat::RowHeader) + header->key_size;
  *data = mmap_->structAt<void>(offset);
  *size = header->data_size;
}

}
}

