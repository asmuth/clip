/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "queryparser.h"
#include "tokenize.h"

namespace fnordmetric {
namespace query {

size_t QueryParser::parse(const char* query, size_t len) {
  const char* cur = query;
  const char* end = cur + len;

  std::vector<Token> token_list;
  tokenizeQuery(&cur, end, &token_list_);

  for (const auto& token : token_list) {
    token.debugPrint();
  }

  return cur - query;
}

}
}
