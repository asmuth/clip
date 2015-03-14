/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_SSTABLECOLUMNREADER_H
#define _FNORD_SSTABLE_SSTABLECOLUMNREADER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <fnord-base/buffer.h>
#include <fnord-base/exception.h>
#include <fnord-base/io/file.h>
#include <fnord-base/io/mmappedfile.h>
#include <fnord-base/util/binarymessagewriter.h>
#include <fnord-sstable/binaryformat.h>
#include <fnord-sstable/fileheaderreader.h>
#include <fnord-sstable/cursor.h>
#include <fnord-sstable/index.h>
#include <fnord-sstable/indexprovider.h>
#include <fnord-sstable/SSTableColumnSchema.h>

namespace fnord {
namespace sstable {

class SSTableColumnReader {
public:

  SSTableColumnReader(SSTableColumnSchema* schema, const Buffer& buf);

  uint32_t getUInt32Column(SSTableColumnID id);
  uint64_t getUInt64Column(SSTableColumnID id);
  double getFloatColumn(SSTableColumnID id);
  String getStringColumn(SSTableColumnID id);

protected:
  SSTableColumnSchema* schema_;
  Buffer buf_;
  util::BinaryMessageReader msg_reader_;
  Vector<std::tuple<SSTableColumnID, uint64_t, uint32_t>> col_data_;
};

} // namespace sstable
} // namespace fnord

#endif
