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

struct SeriesIndexPosition {
  uint64_t series_id;
  uint64_t addr;
  uint64_t size;
};

bool TSDB::commit() {
  std::unique_lock<std::mutex> lk(commit_mutex_);

  /* get a snapshot of all series */
  std::set<uint64_t> series_ids;
  txn_map_.listSlots(&series_ids);

  /* write the pages and partition indexes for each series */
  std::vector<SeriesIndexPosition> series_index_positions;
  for (auto siter = series_ids.begin(); siter != series_ids.end(); ) {
    auto series_id = *siter;

    /* get a snapshot of the series */
    Transaction txn;
    if (!txn_map_.startTransaction(series_id, true, &txn)) {
      /* if we can't start the transaction, the series was deleted while
         our commit is running, so we ignore it */
      siter = series_ids.erase(siter);
      continue;
    } else {
      ++siter;
    }

    /* write the series index data header */
    std::string index_data;
    auto page_idx = txn.getPageIndex();
    writeVarUInt(&index_data, (uint64_t) page_idx->getType());
    writeVarUInt(&index_data, page_idx->getSize());

    /* write each page to disk */
    for (size_t i = 0; i < page_idx->getSize(); ++i) {
      auto page_id = page_idx->getEntries()[i].page_id;

      PageBuffer page_buf(page_idx->getType());
      page_map_.loadPage(page_id, &page_buf);

      std::string page_data;
      page_buf.encode(&page_data);
      assert(!page_data.empty());

      uint64_t page_disk_addr;
      uint64_t page_disk_size;
      if (!allocPage(page_data.size(), &page_disk_addr, &page_disk_size)) {
        return false;
      }

      auto rc = pwrite(fd_, page_data.data(), page_data.size(), page_disk_addr);
      if (rc <= 0) {
        return false;
      }

      writeVarUInt(&index_data, page_disk_addr);
      writeVarUInt(&index_data, page_disk_size);
    }

    for (size_t i = 1; i < page_idx->getSize(); ++i) {
      auto point = page_idx->getSplitpoints()[i - 1].point;
      writeVarUInt(&index_data, point); // FIXME use delta encoding
    }

    SeriesIndexPosition index_pos;
    index_pos.series_id = series_id;
    if (!allocPage(index_data.size(), &index_pos.addr, &index_pos.size)) {
      return false;
    }

    auto rc = pwrite(fd_, index_data.data(), index_data.size(), index_pos.addr);
    if (rc <= 0) {
      return false;
    }

    series_index_positions.emplace_back(index_pos);
  }

  return true;
}

} // namespace tsdb

