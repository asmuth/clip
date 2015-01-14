/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HASH_H
#define _FNORDMETRIC_HASH_H
#include <tuple>

namespace fnord {

template <typename T>
struct hash {
  size_t operator()(const T& value) const;
};

}
#endif
