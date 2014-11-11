/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_ASTUTIL_H
#define _FNORDMETRIC_SQL_ASTUTIL_H
#include <string>
#include <vector>

namespace fnordmetric {
namespace query {
class ASTNode;
class TableRef;

class ASTUtil {
public:

  static std::vector<std::string> columnNamesFromSelectList(
      ASTNode* select_list,
      TableRef* tbl_ref = nullptr);

};

}
}
#endif
