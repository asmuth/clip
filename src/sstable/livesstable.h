/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_LIVESSTABLE_H
#define _FNORD_SSTABLE_LIVESSTABLE_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <fnordmetric/io/file.h>
#include <fnordmetric/io/pagemanager.h>
#include <fnordmetric/sstable/index.h>
#include <fnordmetric/sstable/indexprovider.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnord {
namespace sstable {

/**
 * A sstable that can written (appended) to and read from at the same time.
 *
 *
 *
 *
 * 
 *
 */
class LiveSSTable {
public:

  /**
   * Create and open a new sstable for writing
   */
  static std::unique_ptr<LiveSSTable> create(
      io::File file,
      IndexProvider index_provider,
      void const* header,
      size_t header_size);

  /**
   * Re-open a partially written sstable for writing
   */
  static std::unique_ptr<LiveSSTable> reopen(
      io::File file,
      IndexProvider index_provider);


  LiveSSTable(const LiveSSTable& other) = delete;
  LiveSSTable& operator=(const LiveSSTable& other) = delete;
  ~LiveSSTable();

  /**
   * Append a row to the sstable
   */
  void appendRow(
      void const* key,
      size_t key_size,
      void const* data,
      size_t data_size);

  /**
   * Append a row to the sstable
   */
  void appendRow(
      const std::string& key,
      const std::string& value);

  /**
   * Finalize the sstable (writes out the indexes to disk)
   */
  void finalize();

  /**
   * Get an sstable cursor for the sstable currently being written
   */

protected:
  LiveSSTable(
      io::File&& file,
      std::vector<Index::IndexRef>&& indexes);

  io::File file_;
  std::vector<Index::IndexRef> indexes_;
  std::unique_ptr<io::MmapPageManager> mmap_;
};


}
}

#endif
