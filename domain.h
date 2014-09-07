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
#include <math.h>
#include "../util/format.h"

// FIXPAUL too many copies T val...
namespace fnordmetric {
namespace ui {

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

};

class AnyContinuousDomain {
public:
  virtual void setLogarithmic(bool logarithmic) = 0;
  virtual void setPadding(double min_padding, double top_padding) = 0;
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

template <typename T>
class ContinuousDomain : public Domain<T>, public AnyContinuousDomain {
public:

  /**
   * Create a new numerical domain with explicit parameters
   *
   * @param min_value the smallest value
   * @param max_value the largest value
   * @param logarithmic is this domain a logarithmic domain?
   */
  ContinuousDomain(
    T min_value = std::numeric_limits<T>::max(),
    T max_value = std::numeric_limits<T>::min(),
    bool is_logarithmic = false,
    bool is_inverted = false) :
    min_value_(min_value),
    max_value_(max_value),
    is_logarithmic_(is_logarithmic),
    is_inverted_(is_inverted),
    padding_(0, 0) {}

  double scale(T value) const {
    double scaled;

    if (is_logarithmic_) {
      if (min_value_ < 0) {
        RAISE(
            util::RuntimeException,
            "negative value is outside of logarithmic domain");
      }

      double max_log = 0.0f;
      if (max_value_ >= 1.0) {
        max_log = log10(max_value_ + max_value_* padding_.second);
      }

      double value_log = 0.0f;
      if (value >= 1.0) {
        value_log = log10(value);
      }

      scaled = value_log / max_log;
    } else {
      auto min_max = getRangeWithPadding();
      auto min_value = min_max.first;
      auto max_value = min_max.second;

      scaled = (value - min_value) / (max_value - min_value);
    }

    if (is_inverted_) {
      return 1.0 - scaled;
    } else {
      return scaled;
    }
  }

  std::string label(T value) const {
    return util::format::numberToHuman(value);
  }

  double valueAt(double index) const {
    if (is_logarithmic_) {
      if (max_value_ < 0) {
        RAISE(
            util::RuntimeException,
            "negative value is outside of logarithmic domain");
      }

      double max_log = 0.0f;
      if (max_value_ >= 1.0) {
        max_log = log10(max_value_ + max_value_* padding_.second);
      }

      if (is_inverted_) {
        return pow(10, (1.0 - index) * max_log);
      } else {
        return pow(10, index * max_log);
      }
    } else {
      auto min_max = getRangeWithPadding();
      double min_value = min_max.first;
      double max_value = min_max.second;
      double val_range = min_value + (max_value - min_value);

      if (is_inverted_) {
        return min_value + (max_value - min_value) * (1.0 - index);
      } else {
        return min_value + (max_value - min_value) * index;
      }
    }
  }

  std::pair<double, double> scaleRange(T value) const {
    return std::make_pair(scale(value), scale(value));
  }

  void addValue(const T& value) {
    if (value > max_value_) {
      max_value_ = value;
    }

    if (value < min_value_) {
      min_value_ = value;
    }
  }

  bool contains(T value) const {
    return false;
  }

  const std::vector<double> getTicks() const {
    std::vector<double> ticks;

    double num_ticks = AnyDomain::kDefaultNumTicks;
    for (int n = 0; n < num_ticks; ++n) {
      ticks.push_back((double) n / (double) (num_ticks - 1));
    }

    return ticks;
  }

  const std::vector<std::pair<double, std::string>> getLabels() const {
    auto ticks = getTicks();
    std::vector<std::pair<double, std::string>> labels;

    for (auto tick : ticks) {
      labels.emplace_back(tick,
          util::format::numberToHuman(valueAt(tick)));
    }

    return labels;
  }

  void setMin(T min) {
    min_value_ = min;
    padding_.first = 0.0f;
  }

  void setMax(T max) {
    max_value_ = max;
    padding_.second = 0.0f;
  }

  void setInverted(bool inverted) {
    is_inverted_ = inverted;
  }

  void setLogarithmic(bool logarithmic) {
    is_logarithmic_ = logarithmic;
  }

  void setPadding(double min_padding, double max_padding) {
    padding_.first = min_padding;
    padding_.second = max_padding;
  }


  double min_value_;
  double max_value_;
protected:

  std::pair<double, double> getRangeWithPadding() const {
    double range = max_value_ - min_value_;

    return std::make_pair(
        min_value_ == 0 ? 0 : min_value_ - range * padding_.first,
        max_value_ + range * padding_.second);
  }
  bool is_logarithmic_;
  bool is_inverted_;
  std::pair<double, double> padding_;
};

template <typename T>
class DiscreteDomain : public Domain<T> {
public:

  /**
   * Create a new categorical domain
   */
  DiscreteDomain(bool is_inverted = false) : is_inverted_(is_inverted) {}

  std::string label(T value) const {
    return fnordmetric::util::format::toHuman(value);
  }

  double scale(T value) const {
    size_t index = categories_.end() - std::find(
        categories_.begin(),
        categories_.end(),
        value);

    if (index < 1) {
      RAISE(util::RuntimeException, "can't scale value");
    }

    double cardinality = (double) categories_.size();
    auto scaled = ((double) index - 0.5f) / cardinality;

    if (is_inverted_) {
      return 1.0 - scaled;
    } else {
      return scaled;
    }
  }

  std::pair<double, double> scaleRange(T value) const {
    size_t index = categories_.end() - std::find(
        categories_.begin(),
        categories_.end(),
        value);

    if (index < 1) {
      RAISE(util::RuntimeException, "can't scale value");
    }

    auto cardinality = (double) categories_.size();
    auto begin = (double) (index - 1) / cardinality;;
    auto end = (double) index / cardinality;

    if (is_inverted_) {
      return std::make_pair(1.0 - begin, 1.0 - end);
    } else {
      return std::make_pair(begin, end);
    }
  }

  void addValue(const T& value) {
    addCategory(value);
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

  const std::vector<double> getTicks() const {
    std::vector<double> ticks{0.0};

    for (const auto category : categories_) {
      auto range = scaleRange(category);
      ticks.push_back(range.second);
    }

    return ticks;
  }

  const std::vector<std::pair<double, std::string>> getLabels() const {
    std::vector<std::pair<double, std::string>> labels;

    for (const auto category : categories_) {
      auto point = scale(category);
      labels.emplace_back(point, label(category));
    }

    return labels;
  }

  bool contains(T value) const {
    return std::find(
        categories_.begin(),
        categories_.end(),
        value) != categories_.end();
  }

  void setInverted(bool inverted) {
    is_inverted_ = inverted;
  }

protected:
  bool is_inverted_;
  std::vector<T> categories_;
};

class DomainAdapter {
public:
  DomainAdapter(
      AnyDomain* domain = nullptr) :
      domain_(domain),
      free_on_destroy_(false) {};

  ~DomainAdapter() {
    if (free_on_destroy_) {
      delete domain_;
    }
  }

  AnyDomain* get() const {
    return domain_;
  }

  template <typename T>
  T* getAs() const {
    T* domain = dynamic_cast<T*>(domain_);

    if (domain == nullptr) {
      RAISE(util::RuntimeException, "can't convert domain to requested type");
    }

    return domain;
  }

  bool empty() const {
    return domain_ == nullptr;
  }

  void reset(AnyDomain* domain, bool free_on_destroy = false) {
    if (free_on_destroy_) {
      delete domain_;
    }

    domain_ = domain;
    free_on_destroy_ = free_on_destroy;
  }

  const std::vector<double> getTicks() const {
    if (empty()) {
      abort();
      return std::vector<double>{};
    } else {
      return domain_->getTicks();
    }
  }

  const std::vector<std::pair<double, std::string>> getLabels() const {
    if (empty()) {
      return std::vector<std::pair<double, std::string>>{};
    } else {
      return domain_->getLabels();
    }
  }

protected:
  AnyDomain* domain_;
  bool free_on_destroy_;
};

}
}
#endif
