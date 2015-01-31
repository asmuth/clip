  /**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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
