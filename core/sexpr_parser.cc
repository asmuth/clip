/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <regex>
#include <iostream>
#include "sexpr_parser.h"
#include "stringutil.h"

namespace fviz {

ReturnCode expr_parse_literal(
    const char** cur,
    const char* end,
    ExprStorage* expr) {
  std::string literal;

  for (bool match = true; *cur != end && match;) {
    switch (**cur) {
      case ' ':
      case '\n':
      case '\t':
      case '\r':
      case '\\':
      case '\'':
      case '\"':
      case '(':
      case ')':
        match = false;
        break;
      default:
        literal += **cur;
        (*cur)++;
        break;
    }
  }

  *expr = expr_create_value_literal(std::move(literal));
  return OK;
}

ReturnCode expr_parse_string_escape(
    const char** cur,
    const char* end,
    std::string* str) {
  if (*cur == end || *cur + 1 == end) {
    return error(ERROR, "short escape code");
  }

  auto code = (*cur)[1];
  switch (code) {
    case '\\':
    case '\'':
    case '\"':
      *str += code;
      *cur += 2;
      return OK;
  }

  return error(ERROR, "invalid escape code");
}

ReturnCode expr_parse_string(
    const char** cur,
    const char* end,
    ExprStorage* expr) {
  auto quote_char = *(*cur)++;

  std::string string;
  while (*cur != end && **cur != quote_char) {
    switch (**cur) {
      case '\\':
        if (auto rc = expr_parse_string_escape(cur, end, &string); rc) {
          continue;
        } else {
          return rc;
        }
      default:
        string += *(*cur)++;
        continue;
    }
  }

  if (*cur == end) {
    return error(ERROR, "unterminated string");
  } else {
    ++(*cur);
  }

  *expr = expr_create_value(std::move(string));
  return OK;
}

ReturnCode expr_parse(
    const char* input,
    size_t input_len,
    ExprStorage* expr) {
  auto cur = input;
  auto end = input + input_len;

  std::stack<ExprStorage*> stack;
  stack.push(expr);

  while (cur != end) {
    switch (*cur) {
      case ' ':
      case '\n':
      case '\t':
      case '\r':
        ++cur;
        continue;

      case '(': {
        *stack.top() = expr_create_list();

        auto next = expr_get_next_storage(stack.top()->get());
        auto list = expr_get_list_storage(stack.top()->get());

        stack.pop();
        stack.push(next);
        stack.push(list);

        ++cur;
        continue;
      }

      case ')': {
        if (stack.empty()) {
          return error(ERROR, "unbalanced parens");
        }

        stack.pop();
        ++cur;
        continue;
      }

      case '\'':
      case '"': {
        if (auto rc = expr_parse_string(&cur, end, stack.top()); rc) {
          auto next = expr_get_next_storage(stack.top()->get());
          stack.pop();
          stack.push(next);
          continue;
        } else {
          return rc;
        }
      }

      default:
        if (auto rc = expr_parse_literal(&cur, end, stack.top()); rc) {
          auto next = expr_get_next_storage(stack.top()->get());
          stack.pop();
          stack.push(next);
        } else {
          return rc;
        }
    }
  }

  stack.pop();

  if (!stack.empty()) {
    return error(ERROR, "unbalanced parens");
  }

  return OK;
}

} // namespace fviz
