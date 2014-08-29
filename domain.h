/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_DOMAIN_H
#define _FNORDMETRIC_DOMAIN_H
#include <algorithm>
#include <stdlib.h>
#include <assert.h>
#include "../util/format.h"

namespace fnordmetric {
namespace ui {

/**
 * Untyped domain base class
 */
class AnyDomain {
public:

};

template <typename T>
class Domain : public AnyDomain {
public:
  virtual ~Domain() {}

  /**
   * Returns the label at the specified index
   *
   * @param index the index
   */
  virtual std::string label(T value) const = 0;

  virtual double scale(T value) const = 0;

  virtual std::pair<double, double> scaleRange(T value) const = 0;

  //virtual bool contains(T value) const = 0;

};

template <typename T>
class NumericalDomain : public Domain<T> {
public:
  static const int kDefaultCardinality = 6;

  /**
   * Create a new numerical domain with explicit parameters
   *
   * @param min_value the smallest value
   * @param max_value the largest value
   * @param logarithmic is this domain a logarithmic domain?
   */
  NumericalDomain(
    T min_value,
    T max_value,
    bool is_logarithmic = false) :
    min_value_(min_value),
    max_value_(max_value),
    is_logarithmic_(is_logarithmic) {}

  double scale(T value) const {
    return 0.6;
    /*
    if (value <= min_value_) {
      return 0.0f;
    }

    if (value >= max_value_) {
      return 1.0f;
    }

    return (value - min_value_) / (max_value_ - min_value_);
    */
  }

  std::string label(T value) const {
    return "fu";
    //return util::format::numberToHuman(valueAt(index));
  }

  std::pair<double, double> scaleRange(T value) const {
    return std::make_pair(0,0);
  }

protected:
  double min_value_;
  double max_value_;
  bool is_logarithmic_;
};

template <typename T>
class CategoricalDomain : public Domain<T> {
public:

  /**
   * Create a new categorical domain
   */
  CategoricalDomain() {}

  std::string label(T value) const {
    //if (index < 0 && index > categories_.size() - 1) {
    //  return "n/a";
    //} else {
      return "fnord";
      //return categories_[index];
    //}
  }

  double scale(T value) const {
    size_t index = categories_.end() - std::find(
        categories_.begin(),
        categories_.end(),
        value);

    if (index < 1) {
      RAISE(util::RuntimeException, "can't scale value");
    }

    return index;
  }

  std::pair<double, double> scaleRange(T value) const {
    size_t index = categories_.end() - std::find(
        categories_.begin(),
        categories_.end(),
        value);

    if (index < 1) {
      RAISE(util::RuntimeException, "can't scale value");
    }

    double cardinality = (double) categories_.size();
    return std::make_pair(
        (double) (index - 1) / cardinality,
        (double) index / cardinality);
  }

  void addCategory(const T& category) {
    bool insert = std::find(
        categories_.begin(),
        categories_.end(),
        category) == categories_.end();

    if (insert) {
      categories_.emplace_back(category);
    }
  }

protected:
  std::vector<T> categories_;
};

}
}
#endif
