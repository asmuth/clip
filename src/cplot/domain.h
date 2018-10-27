/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_DOMAIN_H
#define _libstx_DOMAIN_H
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <vector>

// FIXPAUL too many copies T val...
namespace stx {
namespace chart {

/**
 * Untyped domain base class
 */
class AnyDomain {
public:
  static const char kDimensionLetters[];

  // FIXPAUL make this configurable
  static const int kDefaultNumTicks;
  static const double kDefaultDomainPadding;

  enum kDimension {
    DIM_X = 0,
    DIM_Y = 1,
    DIM_Z = 2
  };

  virtual ~AnyDomain() {}

  virtual const std::vector<double> getTicks() const = 0;

  virtual const std::vector<std::pair<double, std::string>> getLabels()
      const = 0;

  virtual void setInverted(bool inverted) = 0;

  virtual void build() = 0;

};

/**
 * Polymorphic domain
 */
template <typename T>
class Domain : public AnyDomain {
public:
  virtual ~Domain() {}

  static Domain<T>* mkDomain();

  /**
   * Returns the label at the specified index
   *
   * @param index the index
   */
  virtual std::string label(T value) const = 0;

  virtual double scale(T value) const = 0;

  virtual std::pair<double, double> scaleRange(T value) const = 0;

  virtual void addValue(const T& value) = 0;

  virtual bool contains(T value) const = 0;

};

}
}
#endif
