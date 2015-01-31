/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2015 Laura Schlimmer, Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORD_BASE_INIPARSER_H_
#define _FNORD_BASE_INIPARSER_H_

#include <tuple>
#include <string>
#include "fnord-base/inspect.h"

namespace fnord {

class IniParser {
public:

  static void start(const String& str);

  static bool test();

};
}

#endif


