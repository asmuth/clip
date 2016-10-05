/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "tsdb.h"
#include "page_index.h"

namespace tsdb {

const size_t TSDB::kMetaBlockSize = 512;
const size_t TSDB::kDefaultBlockSize = 512;
const char TSDB::kMagicBytes[4] = {0x17, 0x42, 0x05, 0x23};

TSDB::TSDB() : fd_(-1), fpos_(0), bsize_(0), txn_map_(&page_map_) {}

TSDB::~TSDB() {}

bool TSDB::open(
    const std::string& filename,
    size_t block_size /* = kDefaultBlockSize */) {
  assert(block_size > 0);

  int oflags = O_CREAT | O_RDWR | O_CLOEXEC | O_EXLOCK | O_EXCL;
  fd_ = ::open(filename.c_str(), oflags, 0666);
  if (fd_ < 0) {
    return false;
  }

  bsize_ = block_size;
  assert(bsize_ >= kMetaBlockSize);
  fpos_ = bsize_;
  return true;
}

bool TSDB::createSeries(
    uint64_t series_id,
    PageType type) {
  if (fd_ < 0) {
    return false;
  }

  std::unique_ptr<PageIndex> page_index(new PageIndex(type));
  page_index->alloc(1);

  auto page_index_entry = page_index->getEntries();
  page_index_entry->page_id = page_map_.allocPage(type);

  return txn_map_.createSlot(
      series_id,
      std::move(page_index));
}

bool TSDB::getCursor(
    uint64_t series_id,
    Cursor* cursor) {
  if (fd_ < 0) {
    return false;
  }

  /* start transaction */
  Transaction txn;
  if (!txn_map_.startTransaction(series_id, true, &txn)) {
    return false;
  }

  *cursor = Cursor(
      txn.getPageIndex()->getType(),
      &page_map_,
      std::move(txn));

  return true;
}

bool TSDB::allocPage(
    uint64_t min_size,
    uint64_t* page_addr,
    uint64_t* page_size) {
  assert(min_size > 0);
  *page_size = ((min_size + bsize_ - 1) / bsize_) * bsize_;
  assert(*page_size > 0);
  *page_addr = fpos_;
  auto new_fpos = fpos_ + *page_size;

  if (ftruncate(fd_, new_fpos) != 0) {
    return false;
  }

  fpos_ = new_fpos;
  return true;
}


} // namespace tsdb

