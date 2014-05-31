/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FILEBACKEND_CURSOR_H
#define _FNORDMETRIC_FILEBACKEND_CURSOR_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include "../backend.h"

namespace fnordmetric {
namespace database {

class StreamRef;

/**
 * This is an internal class. For usage instructions and extended documentation
 * please refer to "storagebackend.h" and "database.h"
 */
class Cursor : public IBackend::IStreamCursor {
public:
  explicit Cursor(std::shared_ptr<StreamRef> stream_ref);
  Cursor(const Cursor& copy) = delete;
  Cursor& operator=(const Cursor& copy) = delete;

  virtual uint64_t seekTo(uint64_t position) override;
  virtual uint64_t seekToFirst() override;
  virtual uint64_t seekToLast() override;
  virtual void getRow(const std::function<void (const uint8_t* data,
      size_t len, uint64_t time)>& func) const override;
  virtual bool next() override;

protected:
  std::shared_ptr<StreamRef> stream_ref_;
};

}
}
#endif
