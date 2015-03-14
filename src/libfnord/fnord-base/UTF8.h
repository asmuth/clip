/*
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_UTF8_H_
#define _FNORD_BASE_UTF8_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <locale>
#include "fnord-base/stdtypes.h"

namespace fnord {

class UTF8 {
public:

  static char32_t nextCodepoint(const char** cur, const char* end);

  static void encodeCodepoint(char32_t codepoint, String* target);

};

}
#endif
