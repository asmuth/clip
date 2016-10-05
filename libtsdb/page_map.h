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
#include <stdlib.h>
#include <memory>
#include <map>
#include "page_buffer.h"

namespace tsdb {

class PageMap {
public:

  using PageIDType = size_t;

  PageMap();
  PageMap(const PageMap& o) = delete;
  PageMap& operator=(const PageMap& o) = delete;
  ~PageMap();

  PageIDType allocPage();

  bool loadPage(PageIDType page_id, PageBuffer* buf);

  bool modifyPage(
      PageIDType page_id,
      std::function<bool (PageBuffer* buf)> fn);

protected:
  PageIDType page_id_;
  std::map<PageIDType, std::unique_ptr<PageBuffer>> buffered_;
};

} // namespace tsdb

