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
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <set>
#include <vector>
#include "tsdb.h"
#include "page_index.h"
#include "varint.h"

namespace tsdb {

bool TSDB::load() {
  uint64_t txn_addr;
  uint64_t txn_size;

  /* read metablock */
  {
    std::string metablock;
    metablock.resize(kMetaBlockSize);

    if (pread(fd_, &metablock[0], metablock.size(), 0) <= 0) {
      return false;
    }

    if (memcmp(metablock.data(), kMagicBytes, sizeof(kMagicBytes)) != 0) {
      return false;
    }

    const char* metablock_cur = &metablock[0] + sizeof(kMagicBytes);
    const char* metablock_end = &metablock[0] + metablock.size();
    if (!readVarUInt(&metablock_cur, metablock_end, &txn_addr)) {
      return false;
    }

    if (!readVarUInt(&metablock_cur, metablock_end, &txn_size)) {
      return false;
    }

    uint64_t fpos;
    if (readVarUInt(&metablock_cur, metablock_end, &fpos)) {
      fpos_ = fpos;
    } else {
      return false;
    }
  }

  /* read transaction */
  std::string txn_data;
  txn_data.resize(txn_size);

#ifdef HAVE_POSIX_FADVISE
  posix_fadvise(fd_, txn_addr, txn_size, POSIX_FADV_NOREUSE);
#endif

  if (pread(fd_, &txn_data[0], txn_size, txn_addr) <= 0) {
    return false;
  }

#ifdef HAVE_POSIX_FADVISE
  posix_fadvise(fd_, txn_addr, txn_size, POSIX_FADV_DONTNEED);
#endif

  const char* txn_data_cur = &txn_data[0];
  const char* txn_data_end = txn_data_cur + txn_data.size();

  uint64_t flags;
  if (!readVarUInt(&txn_data_cur, txn_data_end, &flags)) {
    return false;
  }

  uint64_t bsize;
  if (readVarUInt(&txn_data_cur, txn_data_end, &bsize)) {
    bsize_ = bsize;
  } else {
    return false;
  }

  while (txn_data_cur < txn_data_end) {
    uint64_t series_id;
    if (!readVarUInt(&txn_data_cur, txn_data_end, &series_id)) {
      return false;
    }

    if (series_id == 0) {
      break;
    }

    uint64_t seriesidx_disk_addr;
    uint64_t seriesidx_disk_size;
    if (!readVarUInt(&txn_data_cur, txn_data_end, &seriesidx_disk_addr)) {
      return false;
    }

    if (!readVarUInt(&txn_data_cur, txn_data_end, &seriesidx_disk_size)) {
      return false;
    }

    // FIXME preload next N transaction pages using fadivse(WILLNEED)

    seriesidx_disk_addr *= bsize_;
    seriesidx_disk_size *= bsize_;
    if (!loadTransaction(series_id, seriesidx_disk_addr, seriesidx_disk_size)) {
      return false;
    }
  }

  return true;
}

bool TSDB::loadTransaction(
    uint64_t series_id,
    uint64_t disk_addr,
    uint64_t disk_size) {
  std::string index_data;
  index_data.resize(disk_size);

  if (pread(fd_, &index_data[0], disk_size, disk_addr) <= 0) {
    return false;
  }

#ifdef HAVE_POSIX_FADVISE
  posix_fadvise(fd_, disk_addr, disk_size, POSIX_FADV_DONTNEED);
#endif

  const char* index_data_cur = &index_data[0];
  const char* index_data_end = index_data_cur + index_data.size();

  uint64_t value_size;
  if (!readVarUInt(&index_data_cur, index_data_end, &value_size)) {
    return false;
  }

  uint64_t metadata_len;
  if (!readVarUInt(&index_data_cur, index_data_end, &metadata_len)) {
    return false;
  }

  if (index_data_cur + metadata_len >= index_data_end) { // FIXME overflow
    return false;
  }

  std::string metadata(index_data_cur, metadata_len);
  index_data_cur += metadata_len;

  uint64_t index_len;
  if (!readVarUInt(&index_data_cur, index_data_end, &index_len)) {
    return false;
  }

  std::unique_ptr<PageIndex> page_idx(new PageIndex(value_size, metadata));

  page_idx->setDiskSnapshot(disk_addr, disk_size);

  if (!page_idx->alloc(index_len)) {
    return false;
  }

  for (size_t i = 1; i < index_len; ++i) {
    auto point = &page_idx->getSplitpoints()[i - 1].point;
    if (!readVarUInt(&index_data_cur, index_data_end, point)) {
      return false;
    }
  }

  for (size_t i = 0; i < index_len; ++i) {
    uint64_t page_addr;
    if (!readVarUInt(&index_data_cur, index_data_end, &page_addr)) {
      return false;
    }

    uint64_t page_size;
    if (!readVarUInt(&index_data_cur, index_data_end, &page_size)) {
      return false;
    }

    auto page_id = page_map_.addColdPage(
        value_size,
        page_addr * bsize_,
        page_size * bsize_);

    page_idx->getEntries()[i].page_id = page_id;
  }

  return txn_map_.createSlot(series_id, std::move(page_idx));
}

} // namespace tsdb

