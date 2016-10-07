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
#include <metricd/io/file.h>
#include <metricd/io/mmappedfile.h>
#include <metricd/sstable/binaryformat.h>
#include <metricd/sstable/fileheaderreader.h>
#include <metricd/sstable/cursor.h>
#include <metricd/sstable/index.h>
#include <metricd/sstable/indexprovider.h>
#include <metricd/util/buffer.h>
#include <metricd/util/runtimeexception.h>

namespace fnord {
namespace sstable {

class SSTableReader {
public:
  class SSTableReaderCursor : public sstable::Cursor {
  public:
    SSTableReaderCursor(
        std::shared_ptr<io::MmappedFile> file,
        size_t begin,
        size_t limit);

    void seekTo(size_t body_offset) override;
    bool next() override;
    bool valid() override;
    void getKey(void** data, size_t* size) override;
    void getData(void** data, size_t* size) override;
    size_t position() const override;
  protected:
    std::shared_ptr<io::MmappedFile> mmap_;
    size_t pos_;
    size_t begin_;
    size_t limit_;
  };

  SSTableReader(io::File&& file);
  SSTableReader(const SSTableReader& other) = delete;
  SSTableReader& operator=(const SSTableReader& other) = delete;
  ~SSTableReader();

  /**
   * Get an sstable cursor for the body of this sstable
   */
  std::unique_ptr<SSTableReaderCursor> getCursor();

  void readHeader(const void** data, size_t* size);
  util::Buffer readHeader();
  void readFooter(uint32_t type, void** data, size_t* size);
  util::Buffer readFooter(uint32_t type);

  size_t bodySize() const;
  size_t headerSize() const;

private:
  std::shared_ptr<io::MmappedFile> mmap_;
  uint64_t file_size_;
  FileHeaderReader header_;
};


}
}

#endif
