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
    limit_(limit),
    valid_(false),
    pos_(0),
    seekto_cached_(begin) {
  file_.seekTo(seekto_cached_);
  readRowHeader();
}

void SSTableReader::Cursor::seekTo(size_t body_offset) {
  pos_ = body_offset;
  readRowHeader();
}

void SSTableReader::Cursor::fileSeek(size_t pos) {
  //if (pos == seekto_cached_) {
  //  return;
  //}

  if (pos >= limit_) {
    RAISE(kIndexError, "body offset exceeds limit");
  }

  file_.seekTo(pos);
  seekto_cached_ = pos;
}

bool SSTableReader::Cursor::next() {
  seekTo(pos_ + sizeof(cur_header_) + cur_header_.key_size);
  return valid_;
}

bool SSTableReader::Cursor::valid() {
  return valid_;
}

void SSTableReader::Cursor::getKey(void** data, size_t* size) {
  if (!valid_) {
    RAISE(kIllegalStateError, "invalid cursor");
  }

  if (cur_key_.get() == nullptr) {
    auto buf = new util::Buffer(cur_header_.key_size);
    cur_key_.reset(buf);
    fileSeek(begin_ + pos_ + sizeof(cur_header_));

    size_t bytes_read = file_.read(buf);
    seekto_cached_ += bytes_read;

    if (bytes_read != buf->size()) {
      RAISE(kIOError, "can't read row key");
    }
  }

  *data = cur_key_->data();
  *size = cur_key_->size();
}

void SSTableReader::Cursor::getData(void** data, size_t* size) {
  if (!valid_) {
    RAISE(kIllegalStateError, "invalid cursor");
  }

  if (cur_value_.get() == nullptr) {
    auto buf = new util::Buffer(cur_header_.data_size);
    cur_value_.reset(buf);
    fileSeek(begin_ + pos_ + sizeof(cur_header_) + cur_header_.key_size);

    size_t bytes_read = file_.read(buf);
    seekto_cached_ += bytes_read;

    if (bytes_read != buf->size()) {
      RAISE(kIOError, "can't read row value");
    }
  }

  *data = cur_value_->data();
  *size = cur_value_->size();
}

void SSTableReader::Cursor::readRowHeader() {
  size_t bytes_read = file_.read(&cur_header_, sizeof(cur_header_));
  seekto_cached_ += bytes_read;

  if (bytes_read == sizeof(cur_header_)) {
    valid_ = true;
  } else {
    valid_ = false;
  }
}

}
}

