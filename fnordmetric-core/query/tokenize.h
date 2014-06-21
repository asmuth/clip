/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_TOKENIZE_H
#define _FNORDMETRIC_QUERY_TOKENIZE_H
#include <vector>

namespace fnordmetric {
namespace query {

void tokenizeQuery(
    const char** cur,
    const char* end,
    std::vector<Token>* token_list);

}
}
#endif
