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
#include "tsdb.h"
#include "page_index.h"

namespace tsdb {

bool TSDB::commit() {
  std::unique_lock<std::mutex> lk(commit_mutex_);

  /* get a snapshot of all series */
  std::set<uint64_t> series_ids;
  txn_map_.listSlots(&series_ids);

  /* prepare the partition indexes for each series */
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

    /* write each page to disk */
    auto page_idx = txn.getPageIndex();
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
    }

    /* write the series index to disk */
  }

  return true;
}

} // namespace tsdb

