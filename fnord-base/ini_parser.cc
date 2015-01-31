/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2015 Laura Schlimmer, Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "ini_parser.h"

namespace fnord {

void IniParser::start(const String& str) {
  fnord::iputs("start parsing $0", str);

}

bool IniParser::test() {
  return true;
}


}
