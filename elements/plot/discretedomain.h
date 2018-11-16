/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#ifndef _libstx_DISCRETEDOMAIN_H
#define _libstx_DISCRETEDOMAIN_H
#include "utils/stringutil.h"
#include "domain.h"

namespace signaltk {
namespace chart {

template <typename T>
class DiscreteDomain : public Domain<T> {
public:

  /**
   * Create a new categorical domain
   */
  DiscreteDomain(bool is_inverted = false) : is_inverted_(is_inverted) {}

  std::string label(T value) const {
    return StringUtil::toString(value);
  }

  double scale(T value) const {
    size_t index = categories_.end() - std::find(
        categories_.begin(),
        categories_.end(),
        value);

    if (index < 1) {
      RAISE(kRuntimeError, "can't scale value");
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
      RAISE(kRuntimeError, "can't scale value");
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

  void build() {}

protected:
  bool is_inverted_;
  std::vector<T> categories_;
};

}
}
#endif
