/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_SSTABLEWRITER_H
#define _FNORD_SSTABLE_SSTABLEWRITER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <fnordmetric/io/file.h>
#include <fnordmetric/io/pagemanager.h>
#include <fnordmetric/sstable/cursor.h>
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
class SSTableWriter {
public:

  /**
   * Create and open a new sstable for writing
   */
  static std::unique_ptr<SSTableWriter> create(
      io::File file,
      IndexProvider index_provider,
      void const* header,
      size_t header_size);

  /**
   * Re-open a partially written sstable for writing
   */
  static std::unique_ptr<SSTableWriter> reopen(
      io::File file,
      IndexProvider index_provider);


  SSTableWriter(const SSTableWriter& other) = delete;
  SSTableWriter& operator=(const SSTableWriter& other) = delete;
  ~SSTableWriter();

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
  std::unique_ptr<Cursor> getCursor();

  template <typename IndexType>
  IndexType* getIndex() const;

  size_t bodySize() const;
  size_t headerSize() const;

  void writeIndex(uint32_t index_type, void* data, size_t size);

protected:
  class Cursor : public sstable::Cursor {
  public:
    Cursor(
        SSTableWriter* table,
        io::MmapPageManager* mmap);

    void seekTo(size_t body_offset) override;
    bool next() override;
    bool valid() override;
    void getKey(void** data, size_t* size) override;
    void getData(void** data, size_t* size) override;
  protected:
    std::unique_ptr<io::PageManager::PageRef> getPage();
    SSTableWriter* table_;
    io::MmapPageManager* mmap_;
    size_t pos_;
  };

  SSTableWriter(
      io::File&& file,
      std::vector<Index::IndexRef>&& indexes);

  void writeHeader(void const* data, size_t size);

private:
  void reopen(size_t file_size);

  io::File file_;
  std::vector<Index::IndexRef> indexes_;
  std::unique_ptr<io::MmapPageManager> mmap_;
  size_t header_size_; // FIXPAUL make atomic
  size_t body_size_; // FIXPAUL make atomic
  bool finalized_;
};


}
}

#include "sstablewriter_impl.h"
#endif
