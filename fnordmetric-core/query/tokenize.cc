/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "tokenize.h"

namespace fnordmetric {
namespace query {

void tokenizeQuery(
    const char** cur,
    const char* end,
    std::vector<Token>* token_list) {
  char quote_char = 0;

next:

  /* skip whitespace */
  while (**cur == ' ' && *cur < end) {
    (*cur)++;
  }

  if (*cur >= end) {
    return;
  }

  /* single character tokens */
  switch (**cur) {
    case ';': {
      token_list->emplace_back(Token::T_SEMICOLON);
      (*cur)++;
      return;
    }

    case ',': {
      token_list->emplace_back(Token::T_COMMA);
      (*cur)++;
      goto next;
    }

    case '.': {
      token_list->emplace_back(Token::T_DOT);
      (*cur)++;
      goto next;
    }

    case '(': {
      token_list->emplace_back(Token::T_LPAREN);
      (*cur)++;
      goto next;
    }

    case ')': {
      token_list->emplace_back(Token::T_RPAREN);
      (*cur)++;
      goto next;
    }

    case '=': {
      token_list->emplace_back(Token::T_EQUAL);
      (*cur)++;
      goto next;
    }

    case '+': {
      token_list->emplace_back(Token::T_PLUS);
      (*cur)++;
      goto next;
    }

    case '*': {
      token_list->emplace_back(Token::T_ASTERISK);
      (*cur)++;
      goto next;
    }

    case '"':
    case '\'':
    case '`':
      quote_char = **cur;
      (*cur)++;
      /* fallthrough */
  }

  /* multi char tokens */
  const char* begin = *cur;
  size_t len = 0;

  /* quoted strings */
  if (quote_char) {
    int escape_level = 0;
    while (*cur < end) {
      if (**cur == quote_char) {
        if (escape_level % 2 == 0) break;
      } else if (**cur == '\\') {
        escape_level++;
      } else {
        escape_level = 0;
      }
      (*cur)++;
      len++;
    }
    (*cur)++;
    quote_char = 0;
    token_list->emplace_back(Token::T_STRING, begin, len);
    goto next;
  }

  /* keywords and unquoted strings (i.e table, field names) */
  while (
      **cur != ' ' &&
      **cur != ',' &&
      **cur != '.' &&
      **cur != ';' &&
      **cur != '(' &&
      **cur != ')' &&
      **cur != '=' &&
      **cur != '+' &&
      **cur != '*' &&
      *cur < end) {
    len++;
    (*cur)++;
  }

  Token token(Token::T_STRING, begin, len);

  if (token == "AS") {
    token_list->emplace_back(Token::T_AS);
    goto next;
  }

  if (token == "SELECT") {
    token_list->emplace_back(Token::T_SELECT);
    goto next;
  }

  if (token == "FROM") {
    token_list->emplace_back(Token::T_FROM);
    goto next;
  }

  token_list->push_back(token);
  goto next; // poor mans tail recursion optimization
}

}
}
