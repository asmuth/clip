/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CSVBACKEND_H
#define _FNORDMETRIC_CSVBACKEND_H
#include <memory>
#include <fnordmetric/query/backends/csv/csvtableref.h>

namespace fnordmetric {
namespace query {
class ASTNode;
namespace csv_backend {

class CSVBackend {
public:

  static std::unique_ptr<TableRef> openTable(ASTNode* import);

};

}
}
}
#endif
