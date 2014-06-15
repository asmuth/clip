/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_TRANSACTION_H
#define _FNORDMETRIC_TRANSACTION_H

#include <stdlib.h>
#include <stdint.h>
#include "schema.h"
#include "collection.h"

namespace fnordmetric {
class DocumentRef;
class Cursor;
class Collection;

typedef uint64_t DocumentKey;

/**
 * A transaction object is not threadsafe! If you want to use a cursor from
 * multiple threads you must take care to synchronize access in such a way that
 * no two threads call any method on the cursor object at the same time!
 */
class Transaction {
public:

  /**
   * Start a new transaction on the given collection
   */
  Transaction(Collection* collection);

  Transaction(const Transaction& copy) = delete;
  Transaction& operator=(const Transaction& copy) = delete;

  /**
   * A transaction auto-rollbacks on destruction if it was not comitted
   */
  ~Transaction();

  /**
   * Return a cursor for the collection this transaction is running on and seek
   * to the first document with a key larger than or equal to the specified key.
   *
   * The returned cursor is only valid within this transaction until it is
   * committed or rolled back.
   */
  std::unique_ptr<Cursor> getCursor(const DocumentKey& key);

  /**
   * Return a pointer to the document the cursor is currently pointing to.
   *
   * The returned pointer is valid until the transaction is committed or rolled
   * back.
   */
  //virtual DocumentRef* getDocument(const Cursor* cursor) = 0;

  /**
   * Get the document with the specified document key
   *
   * The returned pointer is valid until the transaction is committed or rolled
   * back.
   */
  DocumentRef* getDocument(const DocumentKey& key);

  /**
   * Create a new document
   *
   * The returned pointer is valid until the transaction is committed or rolled
   * back.
   */
  DocumentRef* createDocument();

  /**
   * Get or create the document with the specified document key
   *
   * The returned pointer is valid until the transaction is committed or rolled
   * back.
   */
  DocumentRef* getOrCreateDocument(const DocumentKey& key);

  /**
   * Commit this transaction.
   */
  bool commit();

  /**
   * Rollback this transaction.
   */
  bool rollback();

  const std::vector<DocumentRef*>& getDirtyDocuments() const;

protected:
  int running_;
  Collection* const collection_;
  std::unique_ptr<Collection::Snapshot> snapshot_;
  std::vector<DocumentRef*> dirty_documents_; /* hrhr ;) */
};

}
#endif
