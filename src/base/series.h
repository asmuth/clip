/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SERIES_H
#define _FNORDMETRIC_SERIES_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {

class Series {
public:
  template <typename T>
  struct Coord {
  public:
    explicit Coord(T value) : value_(value) {}
    explicit Coord(std::nullptr_t);
    T value() const { return value_; }
    bool operator==(const Coord<T>& other) { return value_ == other.value_; }
  protected:
    const T value_;
  };

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

template <typename TX, typename TY>
class Series2D : public Series {
public:
  class Point {
  public:
    explicit Point(TX x, TY y) : data_(Coord<TX>(x), Coord<TY>(y)) {}
    explicit Point(Coord<TX> x, Coord<TY> y) : data_(x, y) {}

    const TX x() const {
      return std::get<0>(data_).value();
    }

    const Coord<TX>& x_coord() const {
      return std::get<0>(data_);
    }

    const TY y() const {
      return std::get<1>(data_).value();
    }

    const Coord<TY>& y_coord() const {
      return std::get<1>(data_);
    }

  protected:
    std::tuple<Coord<TX>, Coord<TY>> data_;
  };

  Series2D() : Series2D("unnamed") {}
  explicit Series2D(const std::string& name) : Series(name) {}

  void addDatum(TX x, TY y) {
    data_.emplace_back(x, y);
  }

  void addDatum(Coord<TX> x, Coord<TY> y) {
    data_.emplace_back(x, y);
  }

  const std::vector<Point>& getData() const {
    return data_;
  }

protected:
  std::vector<Point> data_;
};

template <typename TX, typename TY, typename TZ>
class Series3D : public Series {
public:
  class Point {
  public:
    explicit Point(
        TX x,
        TY y,
        TZ z) :
        data_(Coord<TX>(x), Coord<TY>(y), Coord<TZ>(z)) {}

    explicit Point(
        Coord<TX> x,
        Coord<TY> y,
        Coord<TZ> z) :
        data_(x, y, z) {}

    const TX x() const {
      return std::get<0>(data_).value();
    }

    const Coord<TX>& x_coord() const {
      return std::get<0>(data_);
    }

    const TY y() const {
      return std::get<1>(data_).value();
    }

    const Coord<TY>& y_coord() const {
      return std::get<1>(data_);
    }

    const TZ z() const {
      return std::get<2>(data_).value();
    }

    const Coord<TZ>& z_coord() const {
      return std::get<2>(data_);
    }

  protected:
    std::tuple<Coord<TX>, Coord<TY>, Coord<TZ>> data_;
  };

  Series3D() : Series3D("unnamed") {}
  explicit Series3D(const std::string& name) : Series(name) {}

  void addDatum(TX x, TY y, TZ z) {
    data_.emplace_back(x, y, z);
  }

  void addDatum(Coord<TX> x, Coord<TY> y, Coord<TZ> z) {
    data_.emplace_back(x, y, z);
  }

  const std::vector<Point>& getData() const {
    return data_;
  }

protected:
  std::vector<Point> data_;
};

}
#endif
