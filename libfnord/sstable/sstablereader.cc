/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/exception.h>
#include <fnord/hash/fnv.h>
#include <fnord/sstable/binaryformat.h>
#include <fnord/sstable/sstablereader.h>

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

SSTableReader::~SSTableReader() {
}

void SSTableReader::readHeader(const void** userdata, size_t* userdata_size) {
  header_.readUserdata(userdata, userdata_size);
}

Buffer SSTableReader::readHeader() {
  const void* data;
  size_t size;
  readHeader(&data, &size);

  return Buffer(data, size);
}

void SSTableReader::readFooter(
    uint32_t type,
    void** data,
    size_t* size) {
  size_t pos = header_.headerSize() + header_.bodySize();

  while (pos < file_size_) {
    if (pos + sizeof(BinaryFormat::FooterHeader) > mmap_->size()) {
      RAISE(kIllegalStateError, "footer exceeds file boundary");
    }

    auto footer_header = mmap_->structAt<BinaryFormat::FooterHeader>(pos);
    pos += sizeof(BinaryFormat::FooterHeader);

    if (footer_header->magic != BinaryFormat::kMagicBytes) {
      RAISE(kIllegalStateError, "corrupt sstable footer");
    }

    if (footer_header->footer_size == 0) {
      return;
    }

    if (footer_header->type == type || type == 0) {
      if (pos >= mmap_->size()) {
        RAISE(kIllegalStateError, "footer exceeds file boundary");
      }

      *data = mmap_->structAt<void>(pos);
      *size = footer_header->footer_size;

      if (pos + *size > mmap_->size()) {
        RAISE(kIllegalStateError, "footer exceeds file boundary");
      }

      hash::FNV<uint32_t> fnv;
      auto checksum = fnv.hash(*data, *size);

      if (checksum != footer_header->footer_checksum) {
        RAISE(kIllegalStateError, "footer checksum mismatch. corrupt sstable?");
      }

      if (type != 0) {
        return;
      }
    }

    pos += footer_header->footer_size;
  }
}

Buffer SSTableReader::readFooter(uint32_t type) {
  void* data = nullptr;
  size_t size = 0;
  readFooter(type, &data, &size);
  return Buffer(data, size);
}

std::unique_ptr<SSTableReader::SSTableReaderCursor> SSTableReader::getCursor() {
  auto cursor = new SSTableReaderCursor(
      mmap_,
      header_.headerSize(),
      header_.headerSize() + header_.bodySize());

  return std::unique_ptr<SSTableReaderCursor>(cursor);
}

size_t SSTableReader::bodySize() const {
  return header_.bodySize();
}

size_t SSTableReader::headerSize() const {
  return header_.userdataSize();
}

SSTableReader::SSTableReaderCursor::SSTableReaderCursor(
    std::shared_ptr<io::MmappedFile> mmap,
    size_t begin,
    size_t limit) :
    mmap_(std::move(mmap)),
    begin_(begin),
    limit_(limit),
    pos_(begin) {}

void SSTableReader::SSTableReaderCursor::seekTo(size_t body_offset) {
  pos_ = begin_ + body_offset;
}

size_t SSTableReader::SSTableReaderCursor::position() const {
  return pos_ - begin_;
}

bool SSTableReader::SSTableReaderCursor::next() {
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

bool SSTableReader::SSTableReaderCursor::valid() {
  auto header = mmap_->structAt<BinaryFormat::RowHeader>(pos_);

  auto row_limit = pos_ + sizeof(BinaryFormat::RowHeader) +
      header->key_size +
      header->data_size;

  return header->key_size > 0 && row_limit <= limit_;
}

void SSTableReader::SSTableReaderCursor::getKey(void** data, size_t* size) {
  if (!valid()) {
    RAISE(kIllegalStateError, "invalid cursor");
  }

  auto header = mmap_->structAt<BinaryFormat::RowHeader>(pos_);
  *data = mmap_->structAt<void>(pos_ + sizeof(BinaryFormat::RowHeader));
  *size = header->key_size;
}

void SSTableReader::SSTableReaderCursor::getData(void** data, size_t* size) {
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

