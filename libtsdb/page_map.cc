/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "page_map.h"

namespace tsdb {

PageMap::PageMap() : entries_(nullptr), splitpoints_(nullptr), size_(0) {}

PageMap::~PageMap() {
  if (entries_) {
    free(entries_);
  }
}

} // namespace tsdb

