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
#include <fnordmetric/sstable/livesstable.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnord {
namespace sstable {

std::unique_ptr<LiveSSTable> LiveSSTable::create(
    io::File file,
    IndexProvider index_provider,
    void const* header,
    size_t header_size) {
  if (file.size() > 0) {
    RAISE(kIllegalStateError, "file size must be 0");
  }

  auto sstable = new LiveSSTable(std::move(file), index_provider.popIndexes());
  sstable->writeHeader(header, header_size);
  return std::unique_ptr<LiveSSTable>(sstable);
}

std::unique_ptr<LiveSSTable> LiveSSTable::reopen(
    io::File file,
    IndexProvider index_provider) {
  auto sstable = new LiveSSTable(std::move(file), index_provider.popIndexes());
  return std::unique_ptr<LiveSSTable>(sstable);
}

LiveSSTable::LiveSSTable(
    io::File&& file,
    std::vector<Index::IndexRef>&& indexes) :
    file_(std::move(file)),
    indexes_(std::move(indexes)),
    mmap_(new io::MmapPageManager(file_.fd(), file_.size(), 1)) {}

LiveSSTable::~LiveSSTable() {
}

void LiveSSTable::appendRow(
    void const* key,
    size_t key_size,
    void const* data,
    size_t data_size) {}

void LiveSSTable::appendRow(
    const std::string& key,
    const std::string& value) {
  appendRow(key.data(), key.size(), value.data(), value.size());
}

void LiveSSTable::writeHeader(void const* data, size_t size) {
  auto alloc = mmap_->allocPage(sizeof(BinaryFormat::FileHeader) + size);
  auto page = mmap_->getPage(alloc);

  if (alloc.offset != 0) {
    RAISE(kIllegalStateError, "header page offset must be 0");
  }

  auto header = page->structAt<BinaryFormat::FileHeader>(0);
  header->magic = BinaryFormat::kMagicBytes;
  header->body_size = 0;
  header->header_size = size;

  if (size > 0) {
    auto userdata = page->structAt<void>(sizeof(BinaryFormat::FileHeader));
    memcpy(userdata, data, size);
  }

  page->sync();
}

void LiveSSTable::finalize() {}

}
}
