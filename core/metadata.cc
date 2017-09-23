/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
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

