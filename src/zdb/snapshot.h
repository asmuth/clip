/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <mutex>
#include <map>
#include <set>
#include "page_map.h"
#include "util/autoref.h"

namespace zdb {
class metadata;
class page_map;

struct snapshot : public RefCounted {
  snapshot();
  snapshot(snapshot&& o);
  snapshot(const snapshot& o) = delete;
  snapshot& operator=(const snapshot& o) = delete;
  snapshot& operator=(snapshot&& o);
  ~snapshot();

  metadata* meta;
  page_map* pages;
};

using snapshot_ref = RefPtr<snapshot>;

} // namespace zdb

