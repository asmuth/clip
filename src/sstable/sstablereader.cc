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
    mmap_(new io::MmappedFile(std::move(file))) {
  file_size_ = mmap_->size();

  if (file_size_ < sizeof(BinaryFormat::FileHeader)) {
    RAISE(kIllegalStateError, "not a valid sstable");
  }

  auto header = mmap_->structAt<BinaryFormat::FileHeader>(0);

  if (header->magic != BinaryFormat::kMagicBytes) {
    RAISE(kIllegalStateError, "not a valid sstable");
  }

  body_size_ = header->body_size;
  header_size_ = header->header_size;

  auto meta_file_size =
      sizeof(BinaryFormat::FileHeader) + header_size_ + body_size_;

  if (meta_file_size > file_size_) {
    RAISE(kIllegalStateError, "file metadata offsets exceed file bounds");
  }
}

void SSTableReader::readHeader(void** data, size_t* size) {
  *data = mmap_->structAt<void>(sizeof(BinaryFormat::FileHeader));
  *size = header_size_;
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
  size_t pos = sizeof(BinaryFormat::FileHeader) + header_size_ + body_size_;

  while (pos < file_size_) {
    auto header = mmap_->structAt<BinaryFormat::FooterHeader>(pos);
    pos += sizeof(BinaryFormat::FooterHeader);

    if (header->magic != BinaryFormat::kMagicBytes) {
      RAISE(kIllegalStateError, "corrupt sstable footer");
    }

    if (header->type == type) {
      *data = mmap_->structAt<void>(pos);
      *size = header->footer_size;
      return;
    }

    pos += header->footer_size;
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

