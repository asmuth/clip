/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
namespace fnord {
namespace sstable {

template <typename IndexType>
IndexType* SSTableWriter::getIndex() const {
  for (const auto& idx : indexes_) {
    if (idx->type() == IndexType::kIndexType) {
      auto idx_cast = dynamic_cast<IndexType*>(idx.get());

      if (idx_cast == nullptr) {
        RAISE(kIndexError, "index type collision");
      }

      return idx_cast;
    }
  }

  RAISE(kIndexError, "sstable has no such index");
}

}
}
