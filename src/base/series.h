/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_SERIES_H
#define _FNORDMETRIC_SERIES_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {

template <typename X, typename Y>
class Series2D {
public:

  Series2D(const std::string& name) :
      name_(name) {}


  void addDatum(X x, Y y) {
    data_.emplace_back(x, y);
  }

  const std::string& getName() const {
    return name_;
  }

  const std::vector<std::tuple<X, Y>>& getData() const {
    return data_;
  }

protected:
  std::vector<std::tuple<X, Y>> data_;
  const std::string name_;
};

template <typename X, typename Y, typename Z>
class Series3D {
public:

  Series3D(const std::string& name) :
      name_(name) {}

  void addDatum(X x, Y y, Z z) {
    data_.emplace_back(x, y, z);
  }

  const std::string& getName() const {
    return name_;
  }

  const std::vector<std::tuple<X, Y, Z>>& getData() const {
    return data_;
  }

protected:
  std::vector<std::tuple<X, Y, Z>> data_;
  const std::string name_;
};

}
#endif
