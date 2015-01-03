/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_OPTION_IMPL_H
#define _FNORD_BASE_OPTION_IMPL_H

namespace fnord {

template <typename T>
Option<T>::Option() {}

template <typename T>
Option<T>::Option(const T& value) {}

template <typename T>
Option<T> Some(const T& value) {
  return Option<T>(value);
}

template <typename T>
Option<T> None() {
  return Option<T>();
}

}
#endif
