  /**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_SNAPSHOT_H
#define _FNORDMETRIC_SNAPSHOT_H

namespace fnordmetric {
class Cursor;
class DocumentKey;

/**
 * All methods on a collection instance are threadsafe.
 */
class Snapshot {

  /**
   * Return a cursor for this snapshot and seek to the first document with a
   * key larger than or equal to the specified key.
   */
  std::unique_ptr<Cursor> getCursor(const DocumentKey& key) {}

};

}
#endif
