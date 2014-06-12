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

// FIXPAUL too many token copies!
size_t QueryParser::parse(const char* query, size_t len) {
  ParserState state = {
    .end = query + len,
    .cur = query
  };

  std::vector<Token> token_list;

  while (state.cur < state.end) {
    consumeWhitespace(&state);
    auto token = consumeNextToken(&state);
    token.debugPrint();
    token_list.push_back(token);
  }

  return state.cur - query;
}

/*
QueryParser::Token QueryParser::consumeVerb(ParserState* state) {
  consumeWhitespace(state);

  auto verb = consumeToken(state);
  // FIXPAUL clean this up
  verb.debugPrint();
  if (verb == "SELECT") {
    //  
  }

  while (consumeField()) {

  }

  auto from = consumeFromClause();
}
*/

void QueryParser::consumeWhitespace(ParserState* state) {
  while (*state->cur == ' ') {
    state->cur++;
  }
}

QueryParser::Token QueryParser::consumeNextToken(ParserState* state) {
  switch (*state->cur) {

    case ',': {
      return Token(T_COMMA, state->cur++);
    }

    case ';': {
      return Token(T_SEMICOLON, state->cur++);
    }

    case '.': {
      return Token(T_DOT, state->cur++);
    }

    case '(': {
      return Token(T_LPAREN, state->cur++);
    }

    case ')': {
      return Token(T_RPAREN, state->cur++);
    }

    case '=': {
      return Token(T_EQUAL, state->cur++);
    }

  }

  Token token(T_STRING, state->cur);

  while (
      *state->cur != ' ' &&
      *state->cur != ',' &&
      *state->cur != ';' &&
      *state->cur != '(' &&
      *state->cur != ')' &&
      *state->cur != '=') {
    token.len++;
    state->cur++;
  }

  if (token == "SELECT") {
    return Token(T_SELECT, token.addr);
  }

  if (token == "FROM") {
    return Token(T_FROM, token.addr);
  }

  return token;
}

QueryParser::Token::Token(kTokenType token_type, const char* token_addr) :
    type(token_type),
    addr(token_addr),
    len(0) {}

// FIXPAUL!!
void QueryParser::Token::debugPrint() {
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

  }
}


// FIXPAUL: case insensitive check
bool QueryParser::Token::operator==(const std::string& string) {
  if (this->len != string.size()) {
    return false;
  }

  return strncasecmp(this->addr, string.c_str(), string.size()) == 0;
}

}
}
