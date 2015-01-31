/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_IMPORTSTATEMENT_H
#define _FNORDMETRIC_QUERY_IMPORTSTATEMENT_H
#include <stdlib.h>
#include <vector>
#include <sql/parser/astnode.h>
#include <sql/runtime/compile.h>

namespace fnordmetric {
namespace query {

class ImportStatement {
public:
  ImportStatement(ASTNode* ast, Compiler* compiler);
  const std::string& source_uri() const;
  const std::vector<std::string>& tables() const;
protected:
  std::string source_uri_;
  std::vector<std::string> tables_;
};

}
}
#endif
