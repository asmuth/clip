/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnord/base/exception.h>
#include <fnord/hash/fnv.h>
#include <fnord/sstable/sstablereader.h>
#include <fnord/sstable/sstablerepair.h>

using fnord::Exception;

namespace fnord {
namespace sstable {

SSTableRepair::SSTableRepair(
    const std::string& filename) :
    filename_(filename) {}

bool SSTableRepair::checkAndRepair(bool repair /* = false */) {
  auto file = io::File::openFile(filename_, io::File::O_READ);
  std::unique_ptr<fnord::sstable::SSTableReader> reader_;

  try {
    reader_.reset(new sstable::SSTableReader(std::move(file)));
  } catch (fnord::Exception& rte) {
    fnordmetric::env()->logger()->printf(
        "INFO",
        "SSTableRepair: sstable %s header is corrupt: %s",
        filename_.c_str(),
        rte.getMessage().c_str());

    /* the constructor raises if the checksum is invalid or if the file
     * metadata exceeds the file bounds. there is nothing we can do to recover
     * if that happens */
    return false;
  }

  if (reader_->bodySize() == 0) {
    return checkAndRepairUnfinishedTable(repair);
  } else {
    try {
      void* dummy_data;
      size_t dummy_size;
      reader_->readFooter(0, &dummy_data, &dummy_size);
    } catch (fnord::Exception& rte) {
      fnordmetric::env()->logger()->printf(
          "INFO",
          "SSTableRepair: sstable %s footer is corrupt: %s",
          filename_.c_str(),
          rte.getMessage().c_str());

      return false;
    }
  }

  return true;
}

bool SSTableRepair::checkAndRepairUnfinishedTable(bool repair) {
  io::MmappedFile file(io::File::openFile(filename_, io::File::O_READ));
  FileHeaderReader header_reader(file.data(), file.size());

  if (!header_reader.verify()) {
    return false;
  }

  auto pos = header_reader.headerSize();
  auto end = file.size();

  while (pos < end) {
    if (pos + sizeof(BinaryFormat::RowHeader) > end) {
      break;
    }

    auto row_header = file.structAt<BinaryFormat::RowHeader>(pos);
    auto row_len = sizeof(BinaryFormat::RowHeader) +
        row_header->key_size + row_header->data_size;

    if (pos + row_len > end) {
      break;
    }

    hash::FNV<uint32_t> fnv;
    auto checksum = fnv.hash(
        file.structAt<void>(pos + sizeof(uint32_t)),
        row_len - sizeof(uint32_t));

    if (row_header->checksum != checksum) {
      break;
    }

    pos += row_len;
  }

  if (pos < end) {
    fnordmetric::env()->logger()->printf(
        "INFO",
        "SSTableRepair: found %i extraneous trailing bytes in sstable %s",
        (int) (end - pos),
        filename_.c_str());

    if (repair) {
      fnordmetric::env()->logger()->printf(
          "INFO",
          "SSTableRepair: truncating sstable %s to %i bytes",
          filename_.c_str(),
          (int) pos);

        auto writable_file = io::File::openFile(filename_, io::File::O_WRITE);
        writable_file.truncate(pos);
    } else {
      return false;
    }
  }

  return true;
}

}
}

