/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/sstable/rowoffsetindex.h>

namespace fnord {
namespace sstable {

RowOffsetIndex* RowOffsetIndex::makeIndex() {
  return new RowOffsetIndex();
}

RowOffsetIndex::RowOffsetIndex() : Index(RowOffsetIndex::kIndexType) {}

void RowOffsetIndex::addRow(
    size_t body_offset,
    void const* key,
    size_t key_size,
    void const* data,
    size_t data_size) const {
}

}
}
