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
#include "tsdb.h"
#include "page_index.h"

namespace tsdb {

bool TSDB::insert(
    uint64_t series_id,
    uint64_t time,
    PageType value_type,
    const void* value,
    size_t value_size) {
  /* start transaction */
  Transaction txn;
  if (!txn_map_.startTransaction(series_id, false, &txn)) {
    return false;
  }

  /* find page for insert */
  size_t page_id = 0;
  auto page_idx = txn.getPageIndex();
  assert(page_idx->getSize() > 0);
  page_id = page_idx->getEntries()[0].page_id;

  /* perform page modification */
  auto modify_fn = [time, value, value_size] (PageBuffer* page) -> bool {
    page->insert(time, value, value_size);
    return true;
  };

  assert(page_id > 0);
  page_map_.modifyPage(page_idx->getType(), page_id, modify_fn);

  return true;
}

bool TSDB::insertUInt64(
    uint64_t series_id,
    uint64_t time,
    uint64_t value) {
  return insert(
      series_id,
      time,
      PageType::UINT64,
      &value,
      sizeof(value));
}

} // namespace tsdb

