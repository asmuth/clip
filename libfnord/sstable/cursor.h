/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_CURSOR_H
#define _FNORD_SSTABLE_CURSOR_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnord {
namespace sstable {

class Cursor {
public:
  Cursor();
  Cursor(const Cursor& copy) = delete;
  Cursor& operator=(const Cursor& copy) = delete;
  virtual ~Cursor();

  virtual void seekTo(size_t body_offset) = 0;
  virtual bool next() = 0;
  virtual bool valid() = 0;

  virtual void getKey(void** data, size_t* size) = 0;
  virtual std::string getKeyString();
  virtual void getData(void** data, size_t* size) = 0;
  virtual std::string getDataString();

  virtual size_t position() const = 0;
  virtual size_t nextPosition() = 0;

};


}
}

#endif
