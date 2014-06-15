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

namespace fnordmetric {

/**
 * Neither transaction nor cursor are threadsafe! You must serialize access to
 * individual instances of these classes!
 */
class Transaction {

  /**
   * Create a new transaction. This method should not be called directly but
   * through Collection#startTransaction
   */
  Transaction(std::unique_ptr<Cursor>&& cursor);

  Transaction(const Transaction& copy) = delete;
  Transaction& operator=(const Transaction& copy) = delete;

  /**
   * A transaction auto-rollbacks on destruction if it was not comitted
   */
  ~Transaction();

  /**
   * Return the implicit cursor of this transaction
   */
  Cursor* getCursor();

  /**
   * Commit this transaction.
   */
  bool commit();

  /**
   * Rollback this transaction.
   */
  bool rollback();

protected:
  const std::unique_ptr<Cursor> cursor_;
};

}
#endif
