/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_STRINGUTIL_H_
#define _FNORD_BASE_STRINGUTIL_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {

class StringUtil {
public:

  static void stripTrailingSlashes(std::string* str);

  static void replaceAll(
      std::string* str,
      const std::string& pattern,
      const std::string& replacement);

  static std::string hexPrint(
      const void* data,
      size_t size,
      bool separate_bytes = true,
      bool reverse_byte_order = false);

};

} // namespace fnord

#endif
