/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
namespace fnord {
namespace stats {

template <typename T>
Counter<T>::Counter(const std::string& name) : value_(0) {}

template <typename T>
void Counter<T>::increment(T value) {
  value_ += value;
}

template <typename T>
void Counter<T>::set(T value) {
  value_ = value;
}

template <typename T>
T Counter<T>::value() const {
  return value_;
}


}
}
