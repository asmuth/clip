/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "token.h"

namespace fnordmetric {
namespace query {

static char* copyTokenData(const char* data, size_t len) {
  if (data == nullptr) {
    return nullptr;
  }

  void* data_copy = malloc(len);
  assert(data_copy); // FIXPAUL
  memcpy(data_copy, data, len);
  return static_cast<char *>(data_copy);
}

Token::Token(kTokenType token_type) :
    type_(token_type),
    data_(nullptr),
    len_(0) {}

Token::Token(
    kTokenType token_type,
    const char* data,
    size_t len) :
    type_(token_type),
    len_(len),
    data_(copyTokenData(data, len)) {}

Token::Token(const Token& copy) :
    type_(copy.type_),
    len_(copy.len_),
    data_(copyTokenData(copy.data_, copy.len_)) {}

Token::~Token() {
  if (data_ != nullptr) {
    return free(data_);
  }
}

// FIXPAUL!!
void Token::debugPrint() const {
  switch (type_) {

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
      assert(len_ < sizeof(buf));
      memcpy(buf, data_, len_);
      buf[len_] = 0;
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

bool Token::operator==(const std::string& string) const {
  if (this->len_ != string.size()) {
    return false;
  }

  // FIXPAUL this might copy string just for a strncmp
  return strncasecmp(this->data_, string.c_str(), string.size()) == 0;
}

}
}
