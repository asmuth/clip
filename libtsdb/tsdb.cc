/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "tsdb.h"
#include "page_index.h"

namespace tsdb {

TSDB::TSDB() {}

TSDB::~TSDB() {}

bool TSDB::createSeries(uint64_t series_id) {
  std::unique_ptr<PageIndex> page_index(new PageIndex());
  page_index->alloc(1);

  auto page_index_entry = page_index->getEntries();
  page_index_entry->page_id = page_map_.allocPage();

  return txn_map_.createSlot(
      series_id,
      std::move(page_index));
}

} // namespace tsdb

