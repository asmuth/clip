/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_MEMORYBACKEND_H
#define _FNORDMETRIC_MEMORYBACKEND_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "storagebackend.h"
#include "clock.h"

namespace fnordmetric {

/**
 * Memory Storage Backend for testing
 *
 * This will allocate new memory for every record and never free any of it until
 * it gets destroyed. Also the implementation of seek is O(n) and nothing in here
 * is threadsafe. Don't use this except for testing purposes!
 *
 * See "storagebackend.h" for extended documentation.
 */
class MemoryBackend : public IStorageBackend {
public:

  /**
   * Allocate a new MemoryBackend
   */
  static std::unique_ptr<MemoryBackend> New();

  typedef
    std::function<void (const uint8_t* data, size_t len, uint64_t time)>
    GetRowCB;

  class Cursor : public IStorageCursor {
  public:
    Cursor(const std::shared_ptr<std::vector<IStorageCursor::RowType>>& data);
    virtual uint64_t seekTo(uint64_t position) override;
    virtual uint64_t seekToFirst() override;
    virtual uint64_t seekToLast() override;
    virtual void getRow(const GetRowCB& func) const override;
    virtual bool next();
    virtual uint64_t appendRow(const std::vector<uint8_t>& data) override;
    virtual std::unique_ptr<IStorageCursor> clone() const override;
  protected:
    Cursor(
        const std::shared_ptr<std::vector<IStorageCursor::RowType>>& data,
        int pos);
    const std::shared_ptr<std::vector<IStorageCursor::RowType>> data_;
    int pos_;
  };

  MemoryBackend(const MemoryBackend& copy) = delete;

  virtual std::unique_ptr<IStorageCursor> getCursor(
      const std::string& key) override;

protected:
  MemoryBackend() : IStorageBackend() {}
  std::unordered_map<
      std::string,
      std::shared_ptr<std::vector<IStorageCursor::RowType>>> streams_;
};

}

#endif
