/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_SSTABLEREADER_H
#define _FNORD_SSTABLE_SSTABLEREADER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <fnordmetric/io/file.h>
#include <fnordmetric/io/pagemanager.h>
#include <fnordmetric/sstable/cursor.h>
#include <fnordmetric/sstable/index.h>
#include <fnordmetric/sstable/indexprovider.h>
#include <fnordmetric/util/buffer.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnord {
namespace sstable {

class SSTableReader {
public:

  SSTableReader(io::File&& file);
  SSTableReader(const SSTableReader& other) = delete;
  SSTableReader& operator=(const SSTableReader& other) = delete;

  /**
   * Get an sstable cursor for the body of this sstable
   */
  //std::unique_ptr<Cursor> getBodyCursor();

  //template <typename IndexType>
  //IndexType* getIndex() const;

  util::Buffer readHeader();
  util::Buffer readFooter(uint32_t type);

  size_t bodySize() const;
  size_t headerSize() const;

protected:
/*
  class Cursor : public sstable::Cursor {
  public:

    void seekTo(size_t body_offset) override;
    bool next() override;
    bool valid() override;
    void getKey(void** data, size_t* size) override;
    void getData(void** data, size_t* size) override;
  protected:
    std::unique_ptr<io::PageManager::PageRef> getPage();
    LiveSSTable* table_;
    io::MmapPageManager* mmap_;
    size_t pos_;
  };
*/
private:
  io::File file_;

  uint64_t file_size_;
  uint64_t body_size_;
  uint32_t header_size_;
};


}
}

#endif
