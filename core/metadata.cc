/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metadata.h"

namespace zdb {

column_block::column_block() : present(false), page(nullptr) {}

row_block::row_block(
    const column_list& table) :
    columns(table.size()),
    row_count(0) {}

metadata::metadata() : dirty(false) {}

} // namespace zdb

