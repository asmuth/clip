/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "queryparser.h"

namespace fnordmetric {
namespace query {

size_t QueryParser::parse(const char* query, size_t len) {
  const char* cur = query;
  const char* end = cur + len;

  std::vector<Token> token_list;
  tokenize(&cur, end, &token_list_);

  for (const auto& token : token_list) {
    token.debugPrint();
  }

  return cur - query;
}

void QueryParser::tokenize(
    const char** cur,
    const char* end,
    std::vector<Token>* token_list) {
next:
  char quote_char = 0;

  if (token_list->size() > 0) {
    token_list->back().debugPrint();
  }

  /* skip whitespace */
  while (**cur == ' ' && *cur < end) {
    (*cur)++;
  }

  if (*cur >= end) {
    return;
  }

  /* single character tokens */
  switch (**cur) {
    case ',': {
      token_list->emplace_back(T_COMMA, (*cur)++);
      goto next;
    }

    case ';': {
      token_list->emplace_back(T_SEMICOLON, (*cur)++);
      return;
    }

    case '.': {
      token_list->emplace_back(T_DOT, (*cur)++);
      goto next;
    }

    case '(': {
      token_list->emplace_back(T_LPAREN, (*cur)++);
      goto next;
    }

    case ')': {
      token_list->emplace_back(T_RPAREN, (*cur)++);
      goto next;
    }

    case '=': {
      token_list->emplace_back(T_EQUAL, (*cur)++);
      goto next;
    }

    case '+': {
      token_list->emplace_back(T_PLUS, (*cur)++);
      goto next;
    }

    case '"':
    case '\'':
    case '`':
      quote_char = **cur;
      (*cur)++;
      /* fallthrough */
  }

  /* string tokens */
  Token token(T_STRING, *cur);

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
      token.len++;
    }
    (*cur)++;
    quote_char = 0;
  } else {
    while (
        **cur != ' ' &&
        **cur != ',' &&
        **cur != '.' &&
        **cur != ';' &&
        **cur != '(' &&
        **cur != ')' &&
        **cur != '=' &&
        **cur != '+' &&
        *cur < end) {
      token.len++;
      (*cur)++;
    }

    if (token == "SELECT") {
      token_list->emplace_back(T_SELECT, token.addr);
      goto next;
    }

    if (token == "FROM") {
      token_list->emplace_back(T_FROM, token.addr);
      goto next;
    }
  }

  token_list->push_back(token);
  goto next; // poor mans tail recursion optimization
}

QueryParser::Token::Token(kTokenType token_type, const char* token_addr) :
    type(token_type),
    addr(token_addr),
    len(0) {}

// FIXPAUL!!
void QueryParser::Token::debugPrint() const {
  switch (type) {

    case T_SELECT:
      printf("T_SELECT\n");
      break;

    case T_FROM:
      printf("T_FROM\n");
      break;

    case T_COMMA:
      printf("T_COMMA\n");
      break;

    case T_DOT:
      printf("T_DOT\n");
      break;

    case T_STRING: {
      char buf[1024];
      assert(this->len < sizeof(buf));
      memcpy(buf, this->addr, this->len);
      buf[this->len] = 0;
      printf("T_STRING(%s)\n", buf);
      break;
    }

    case T_SEMICOLON:
      printf("T_SEMICOLON\n");
      break;

    case T_LPAREN:
      printf("T_LPAREN\n");
      break;

    case T_RPAREN:
      printf("T_RPAREN\n");
      break;

    case T_EQUAL:
      printf("T_EQUAL\n");
      break;

    case T_PLUS:
      printf("T_PLUS\n");
      break;

  }
}


// FIXPAUL: case insensitive check
bool QueryParser::Token::operator==(const std::string& string) const {
  if (this->len != string.size()) {
    return false;
  }

  return strncasecmp(this->addr, string.c_str(), string.size()) == 0;
}

}
}
