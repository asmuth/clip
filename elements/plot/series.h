/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _libstx_SERIES_H
#define _libstx_SERIES_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <utils/exception.h>
#include <utils/stringutil.h>

namespace plotfx {
namespace chart {

class Series {
public:
  enum kProperty {
    P_COLOR = 1,
    P_LABEL = 2,
    P_LINE_STYLE = 3,
    P_LINE_WIDTH = 4,
    P_POINT_STYLE = 5,
    P_POINT_SIZE = 6
  };

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

  class AnyPoint {
  public:
    AnyPoint() {}

    int getPropertyOverride(kProperty prop) const {
      for (const auto& override : prop_overrides_) {
        if (override.first == prop) {
          return override.second;
        }
      }

      return -1;
    }

    void setPropertyOverride(kProperty prop, int index) {
      for (auto& override : prop_overrides_) {
        if (override.first == prop) {
          override.second = index;
          return;
        }
      }

      prop_overrides_.emplace_back(prop, index);
    }

  protected:
    std::vector<std::pair<kProperty, int>> prop_overrides_;
  };

  Series(const std::string& name) :
      name_(name) {}

  const std::string& name() const {
    return name_;
  }

  const std::string& getProperty(kProperty prop) const {
    const auto p = properties_.find(prop);

    if (p != properties_.end()) {
      if (p->second.size() > 0) {
        return p->second[0];
      }
    }

    RAISE(kRuntimeError, "property not set");
  }

  const std::string& getProperty(kProperty prop, AnyPoint const* point) const {
    auto override = point->getPropertyOverride(prop);

    if (override > 0) {
      const auto p = properties_.find(prop);
      if (p != properties_.end()) {
        if (p->second.size() > override) {
          return p->second[override];
        }
      }
    }

    return getProperty(prop);
  }

  bool hasProperty(kProperty prop) const {
    return properties_.find(prop) != properties_.end();
  }

  void setProperty(
      kProperty prop,
      AnyPoint* point,
      const std::string& val) {
    if (!hasProperty(prop)) {
      setDefaultProperty(prop, val);
    }

    if (getProperty(prop) == val) {
      return;
    }

    auto p = properties_.find(prop);
    p->second.emplace_back(val);
    point->setPropertyOverride(prop, p->second.size() - 1);
  }

  void setDefaultProperty(kProperty prop, const std::string& val) {
    const auto p = properties_.find(prop);

    if (p == properties_.end()) {
      properties_.emplace(std::make_pair(prop, std::vector<std::string>{val}));
    }
  }

protected:
  std::unordered_map<int, std::vector<std::string>> properties_;
  const std::string name_;
};

template <typename TX, typename TY>
class Series2D : public Series {
public:
  class Point : public AnyPoint {
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

  std::vector<Point>& getData() {
    return data_;
  }

  std::string labelFor(const Point* const point) const {
    if (hasProperty(P_LABEL)) {
      return getProperty(P_LABEL, point);
    }

    return StringUtil::format("$0: $1", point->x(), point->y());
  }

protected:
  std::vector<Point> data_;
};

template <typename TX, typename TY, typename TZ>
class Series3D : public Series {
public:
  class Point : public AnyPoint {
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

  std::vector<Point>& getData() {
    return data_;
  }

  std::string labelFor(const Point* const point) const {
    if (hasProperty(P_LABEL)) {
      return getProperty(P_LABEL, point);
    }

    return StringUtil::format("$0: $1, $2", point->x(), point->y(), point->z());
  }

protected:
  std::vector<Point> data_;
};

}
}
#endif
