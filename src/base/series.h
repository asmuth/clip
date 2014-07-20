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

class Series {
public:
  Series(const std::string& name) :
      name_(name) {}

  const std::string& getName() const {
    return name_;
  }

  void setColor(const std::string& color) {
    color_ = color;
  }

  const std::string& getColor() const {
    return color_;
  }

protected:
  const std::string name_;
  std::string color_;
};

template <typename X, typename Y>
class Series2D : public Series {
public:
  Series2D() : Series2D("unnamed") {}
  Series2D(const std::string& name) : Series(name) {}

  void addDatum(X x, Y y) {
    data_.emplace_back(x, y);
  }

  const std::vector<std::tuple<X, Y>>& getData() const {
    return data_;
  }

protected:
  std::vector<std::tuple<X, Y>> data_;
};

template <typename X, typename Y, typename Z>
class Series3D : public Series {
public:
  Series3D() : Series3D("unnamed") {}
  Series3D(const std::string& name) : Series(name) {}

  void addDatum(X x, Y y, Z z) {
    data_.emplace_back(x, y, z);
  }

  const std::vector<std::tuple<X, Y, Z>>& getData() const {
    return data_;
  }

protected:
  std::vector<std::tuple<X, Y, Z>> data_;
};

}
#endif
