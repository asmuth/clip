/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include "pageindex.h"

namespace fnordmetric {

PageIndex::PageIndex(const PageManager::Page& index_page) :
    index_page_(index_page) {}

PageIndex::~PageIndex() {
  printf("page index freed!\n");
}

PageIndex* PageIndex::clone() {
  return new PageIndex(index_page_);
}

}
