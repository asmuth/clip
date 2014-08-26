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

template <typename T>
class Domain {
public:
  virtual ~Domain() {}

  /**
   * Returns the label at the specified index
   *
   * @param index the index
   */
  virtual std::string labelAt(int index) const = 0;

  /**
   * Returns the 0-1 axis offset of the index
   *
   * @param index the index
   */
  virtual double offsetAt(int index) const = 0;

  virtual double scale(T value) const = 0;

  /**
   * Returns the number of indexes of this domain
   */
  virtual int getCardinality() const = 0;

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
    double min_value,
    double max_value,
    int cardinality = kDefaultCardinality,
    bool is_logarithmic = false) :
    min_value_(min_value),
    max_value_(max_value),
    cardinality_(cardinality),
    is_logarithmic_(is_logarithmic) {}

  double scale(T value) const override;

/*
  double scale(double value) const {
    if (value <= min_value_) {
      return 0.0f;
    }

    if (value >= max_value_) {
      return 1.0f;
    }

    return (value - min_value_) / (max_value_ - min_value_);
  }
*/

  double valueAt(int index) const {
    return min_value_ + (max_value_ - min_value_) * offsetAt(index);
  }

  double offsetAt(int index) const {
    return (double) index / (double) cardinality_;
  }

  std::string labelAt(int index) const override {
    return util::format::numberToHuman(valueAt(index));
  }

  int getCardinality() const override {
    return cardinality_ + 1;
  }

protected:
  double min_value_;
  double max_value_;
  int cardinality_;
  bool is_logarithmic_;
};

template <typename T>
class CategoricalDomain : public Domain<T> {
public:

  /**
   * Create a new categorical domain
   */
  CategoricalDomain() {}

  std::string labelAt(int index) const override {
    if (index < 0 && index > categories_.size() - 1) {
      return "n/a";
    } else {
      return categories_[index];
    }
  }

  double scale(T value) const override;

  double offsetAt(int index) const override {
    return (double) index / categories_.size() + (1 / categories_.size()) * 0.5;
  }

  int getCardinality() const override {
    return categories_.size();
  }

  void addCategory(const std::string& category) {
    bool exists = std::find(
        categories_.begin(),
        categories_.end(),
        category) == categories_.end();

    if (exists) {
      categories_.emplace_back(category);
    }
  }

protected:
  std::vector<std::string> categories_;
};

}
}
#endif
