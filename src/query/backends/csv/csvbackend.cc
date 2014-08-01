/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <memory>
#include <fnordmetric/query/astnode.h>
#include <fnordmetric/query/backends/csv/csvbackend.h>
#include <fnordmetric/query/backends/csv/csvtableref.h>
#include <fnordmetric/query/token.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {
namespace csv_backend {

std::unique_ptr<TableRef> CSVBackend::openTable(ASTNode* import) {
  char column_sep = ',';
  char row_sep = '\n';
  char quote_char = '"';
  char escape_char = '\\';
  bool headers = false;
  auto filename = import->getChildren()[1]->getToken()->getString();

  for (int i = 2; i < import->getChildren().size(); ++i) {
    const auto& child = import->getChildren()[i];

    switch (child->getToken()->getType()) {
      case Token::T_HEADER:
        headers = true;
        break;

      default:
        RAISE(
            util::RuntimeException,
            "invalid option for CSV table: %s",
            Token::getTypeName(child->getToken()->getType()));
    }
  }

  auto csv = CSVInputStream::openFile(
      filename,
      column_sep,
      row_sep,
      quote_char);

  return std::unique_ptr<TableRef>(
      new CSVTableRef(std::move(csv), headers));
}


}
}
}
