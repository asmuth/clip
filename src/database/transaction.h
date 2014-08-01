/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_TRANSACTION_H
#define _FNORDMETRIC_TRANSACTION_H

#include <stdlib.h>
#include <stdint.h>
#include "collection.h"
#include "documentkey.h"

namespace fnordmetric {
class DocumentRef;
class Cursor;
class Collection;
class Snapshot;

/**
 * A transaction object is not threadsafe! If you want to use a cursor from
 * multiple threads you must take care to synchronize access in such a way that
 * no two threads call any method on the cursor object at the same time!
 */
class Transaction {
public:

  /**
   * Start a new transaction
   */
  Transaction(std::unique_ptr<Snapshot> snapshot);

  Transaction(const Transaction& copy) = delete;
  Transaction& operator=(const Transaction& copy) = delete;

  /**
   * A transaction auto-rollbacks on destruction if it was not comitted
   */
  ~Transaction();

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
  DocumentRef* createDocument(const DocumentKey& key = DocumentKey(0));

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

  /**
   * Return a cursor for the collection this transaction is running on and seek
   * to the first document with a key larger than or equal to the specified key.
   *
   * The returned cursor is only valid within this transaction until it is
   * committed or rolled back.
   */
  std::unique_ptr<Cursor> getCursor(const DocumentKey& key);

  /**
   * Return a list of all documents that have been touched in this transaction.
   *
   * The returned pointers are only valid within this transaction until it is
   * committed or rolled back.
   */
  const std::vector<DocumentRef*>& getDirtyDocuments() const;

protected:
  int running_;
  std::unique_ptr<Snapshot> snapshot_;
  std::vector<DocumentRef*> dirty_documents_; /* hrhr ;) */
};

}
#endif
