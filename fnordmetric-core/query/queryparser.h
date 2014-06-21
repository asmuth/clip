/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_QUERYPARSER_H
#define _FNORDMETRIC_QUERY_QUERYPARSER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include "token.h"

namespace fnordmetric {
namespace query {

class QueryParser {
  friend class QueryTest;
public:

  /**
   * Parse a single SQL Query from the input string. Returns the number of bytes
   * that have been consumed.
   */
  size_t parse(const char* query, size_t len);

protected:

  std::vector<Token> token_list_;
};

}
}
#endif
