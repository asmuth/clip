/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_QUERY_TOKENIZE_H
#define _FNORDMETRIC_QUERY_TOKENIZE_H
#include <vector>
#include "token.h"

namespace fnordmetric {
namespace query {

void tokenizeQuery(
    const char** cur,
    const char* end,
    std::vector<Token>* token_list);

void tokenizeQuery(
    const std::string& query,
    std::vector<Token>* token_list);

}
}
#endif
