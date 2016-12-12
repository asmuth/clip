/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V. <laura@eventql.io>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <string>
#include <list>
#include <vector>
#include <utility>
#include "metricd/util/return_code.h"

namespace fnordmetric {

enum class DataFrameType {
  UINT, INT, DOUBLE
};

/**
 * A named vector of numeric values and an optional corresponding list of
 * timestamps
 */
class DataFrame {
public:

  DataFrame(DataFrameType type, bool has_time);
  DataFrame(DataFrame&& other);
  DataFrame(const DataFrame& other) = delete;
  ~DataFrame();

  DataFrame& operator=(DataFrame&& other);
  DataFrame& operator=(const DataFrame& other) = delete;

  const std::vector<std::string> getID() const;
  void setID(const std::vector<std::string> id);

  template <typename T>
  const T* getData() const;

  template <typename T>
  T* getData();

  const uint64_t* getTime() const;
  uint64_t* getTime();
  bool hasTime() const;

  size_t getSize() const;
  size_t getEntrySize() const;

  void resize(size_t len);

  template <typename T>
  void addValue(T value, uint64_t time = 0);

  void debugPrint() const;

protected:
  DataFrameType type_;
  bool has_time_;
  std::vector<std::string> id_;
  void* data_;
  size_t size_;
  size_t capacity_;
};

class DataFrameBundle {
public:

  DataFrameBundle();
  DataFrameBundle(const DataFrameBundle& other) = delete;
  DataFrameBundle(DataFrameBundle&& other);

  DataFrameBundle& operator=(DataFrameBundle&& other);
  DataFrameBundle& operator=(const DataFrameBundle& other) = delete;

  size_t getFrameCount() const;
  const DataFrame* getFrame(size_t idx) const;
  DataFrame* addFrame(DataFrameType type, bool has_time);

  void debugPrint() const;

protected:
  std::vector<DataFrame> frames_;
};

} // namespace fnordmetric

#include "data_frame_impl.h"

