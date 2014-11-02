/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_INDEX_H
#define _FNORD_SSTABLE_INDEX_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnord {
namespace sstable {

class Index {
public:
  typedef std::unique_ptr<Index> IndexRef;

  Index(uint32_t type);
  Index(const Index& copy) = delete;
  Index& operator=(const Index& copy) = delete;

  uint32_t type() const;

  virtual void addRow(
      size_t body_offset,
      void const* key,
      size_t key_size,
      void const* data,
      size_t data_size) const = 0;

protected:
  uint32_t type_;
};


}
}

#endif
