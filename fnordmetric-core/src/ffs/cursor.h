/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_CURSOR_H
#define _FNORDMETRIC_CURSOR_H
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include "pagemanager.h"

namespace fnordmetric {

/**
 * A storage cursor is a stateful iterator for a single dollection.
 *
 */
class Cursor {
public:
  Cursor(const Cursor& copy) = delete;
  Cursor& operator=(const Cursor& copy) = delete;
  virtual ~Cursor();

  /**
   * Try to advance the cursor by n documents. Returns the number of documents
   * the cursor was advanced by
   */
  virtual size_t advanceBy(size_t n) = 0;

protected:
  //StreamRef* stream_ref_;
  //std::shared_ptr<const PageAlloc> current_page_;
  //std::unique_ptr<PageManager::PageRef> current_page_ref_;
  //uint64_t current_page_offset_;
  //size_t current_page_index_;
  //std::shared_ptr<PageManager> page_manager_;
};

}
#endif
