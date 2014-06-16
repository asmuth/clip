/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_DOCUMENTREF_H
#define _FNORDMETRIC_DOCUMENTREF_H
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include "documentkey.h"

namespace fnordmetric {
class PageManager;
class Collection;

/**
 */
class DocumentRef {
public:

  /**
   * Create a new DocumentRef for an empty document
   */
  explicit DocumentRef(Collection* collection, const DocumentKey& key);

  void revert();
  bool isDirty();

  DocumentRef(const DocumentRef& copy) = delete;
  DocumentRef& operator=(const DocumentRef& copy) = delete;

  void setDocumentKey(const DocumentKey& key);
  const DocumentKey& getDocumentKey() const;

protected:
  DocumentKey key_;
  Collection* collection_;
  int dirty_;
  //StreamRef* stream_ref_;
  //std::shared_ptr<const PageAlloc> current_page_;
  //std::unique_ptr<PageManager::PageRef> current_page_ref_;
  //uint64_t current_page_offset_;
  //size_t current_page_index_;
  //std::shared_ptr<PageManager> page_manager_;
};

}
#endif
