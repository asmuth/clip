/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include "fnv.h"

namespace fnord {
namespace util {

/**
 * Parameters for the 32bit version of the FNV hash function
 *
 * basis: 2166136261
 * prime: 16777619
 *
 */
template<> FNV<uint32_t>::FNV() :
    basis_(2166136261llu),
    prime_(16777619llu) {}


/**
 * Parameters for the 64bit version of the FNV hash function
 *
 * basis: 14695981039346656037
 * prime: 1099511628211
 *
 */
template<> FNV<uint64_t>::FNV() :
    basis_(14695981039346656037llu),
    prime_(1099511628211llu) {}

}
}
