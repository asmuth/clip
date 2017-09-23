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
#include "zdb.h"
#include "lock.h"
#include "database.h"

namespace zdb {

zdb_err_t put_raw(
    database_ref db,
    const std::string& table_name,
    const void** tuple_vals,
    const size_t* tuple_lengths,
    size_t tuple_count) {
  assert(!!db);

  /* check that database is not readonly */
  if (db->readonly) {
    return ZDB_ERR_READONLY;
  }

  /* acquire write lock */
  lock_guard lk(&db->lock);
  lk.lock_write();

  /* find table */
  auto table_iter = db->meta.tables.find(table_name);
  if (table_iter == db->meta.tables.end()) {
    return ZDB_ERR_NOTFOUND;
  }

  auto& table = table_iter->second;

  /* find or create row block */
  row_block* rblock = nullptr;
  if (table.row_map.empty()) {
    table.row_map.emplace_back(row_block(table.columns));
    rblock = &table.row_map.back();
  } else {
    rblock = &table.row_map.back();
  }

  /* add values to columns */
  for (size_t i = 0; i < std::min(tuple_count, table.columns.size()); ++i) {
    /* ensure page is loaded*/
    if (!rblock->columns[i].page) {
      rblock->columns[i].page = page_malloc(table.columns[i].type);
    }

    rblock->columns[i].page->append(tuple_vals[i], tuple_lengths[i]);
  }

  rblock->row_count++;
  table.row_count++;

  return ZDB_SUCCESS;
}

} // namespace zdb

