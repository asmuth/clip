/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    return ReturnCode::error("EARG", "short escape code");
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

  return ReturnCode::error("EARG", "invalid escape code");
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
    return ReturnCode::error("EARG", "unterminated string");
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
          return ReturnCode::error("EARG", "unbalanced parens");
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
    return ReturnCode::error("EARG", "unbalanced parens");
  }

  return OK;
}

} // namespace fviz
