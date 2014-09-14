/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_BACKEND_H
#define _FNORDMETRIC_SQL_BACKEND_H
#include <memory>

namespace fnordmetric {
namespace query {

class Backend {
public:

  //static std::unique_ptr<TableRef> openTable(ASTNode* import);

  virtual std::string protocol() const = 0;

};

}
}
#endif
