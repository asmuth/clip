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

    fpos_ = txn_addr + txn_size;
  }

  /* read transaction */
  std::string txn_data;
  txn_data.resize(txn_size);

  if (pread(fd_, &txn_data[0], txn_size, txn_addr) <= 0) {
    return false;
  }

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
  return true;
}

} // namespace tsdb

