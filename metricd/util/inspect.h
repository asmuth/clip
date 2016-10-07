/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_INSPECT_H_
#define _FNORDMETRIC_UTIL_INSPECT_H_
#include <string>
#include <vector>

template <typename T>
std::string inspect(const T& value);

template <typename T1, typename T2>
std::string inspect(const std::pair<T1, T2>& value);

template <typename T>
std::string inspect(const std::vector<T>& value);

#include "inspect_impl.h"
#endif
