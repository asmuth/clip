/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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
  Token::kTokenType string_type = Token::T_STRING;

next:

  /* skip whitespace */
  while ((
      **cur == ' ' ||
      **cur == '\t' ||
      **cur == '\n' ||
      **cur == '\r')
      && *cur < end) {
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
      goto next;
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

    case '-': {
      token_list->emplace_back(Token::T_MINUS);
      (*cur)++;
      goto next;
    }

    case '*': {
      token_list->emplace_back(Token::T_ASTERISK);
      (*cur)++;
      goto next;
    }

    case '!': {
      token_list->emplace_back(Token::T_BANG);
      (*cur)++;
      goto next;
    }

    case '/': {
      token_list->emplace_back(Token::T_SLASH);
      (*cur)++;
      goto next;
    }

    case '^': {
      token_list->emplace_back(Token::T_CIRCUMFLEX);
      (*cur)++;
      goto next;
    }

    case '~': {
      token_list->emplace_back(Token::T_TILDE);
      (*cur)++;
      goto next;
    }

    case '%': {
      token_list->emplace_back(Token::T_PERCENT);
      (*cur)++;
      goto next;
    }

    case '&': {
      token_list->emplace_back(Token::T_AMPERSAND);
      (*cur)++;
      goto next;
    }

    case '|': {
      token_list->emplace_back(Token::T_PIPE);
      (*cur)++;
      goto next;
    }

    case '<': {
      token_list->emplace_back(Token::T_LT);
      (*cur)++;
      goto next;
    }

    case '>': {
      token_list->emplace_back(Token::T_GT);
      (*cur)++;
      goto next;
    }

    /* numeric literals */
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      const char* begin = *cur;
      for (; (**cur >= '0' && **cur <= '9') || **cur == '.'; (*cur)++);
      token_list->emplace_back(Token::T_NUMERIC, begin, *cur - begin);
      goto next;
    }

    case '`':
      string_type = Token::T_IDENTIFIER;
      /* fallthrough */

    case '"':
    case '\'':
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
    token_list->emplace_back(string_type, begin, len);
    quote_char = 0;
    string_type = Token::T_STRING;
    goto next;
  }

  /* keywords and identifiers (i.e table, field names) */
  while (
      **cur != ' ' &&
      **cur != '\t' &&
      **cur != '\n' &&
      **cur != '\r' &&
      **cur != ',' &&
      **cur != '.' &&
      **cur != ';' &&
      **cur != '(' &&
      **cur != ')' &&
      **cur != '=' &&
      **cur != '+' &&
      **cur != '-' &&
      **cur != '*' &&
      **cur != '!' &&
      **cur != '/' &&
      **cur != '^' &&
      **cur != '~' &&
      **cur != '%' &&
      **cur != '&' &&
      **cur != '|' &&
      *cur < end) {
    len++;
    (*cur)++;
  }

  // FIXPAUL this should be a hashmap/trie lookup!
  Token token(Token::T_IDENTIFIER, begin, len);

  if (token == "AS") {
    token_list->emplace_back(Token::T_AS);
    goto next;
  }

  if (token == "ASC") {
    token_list->emplace_back(Token::T_ASC);
    goto next;
  }

  if (token == "DESC") {
    token_list->emplace_back(Token::T_DESC);
    goto next;
  }

  if (token == "NOT") {
    token_list->emplace_back(Token::T_NOT);
    goto next;
  }

  if (token == "TRUE") {
    token_list->emplace_back(Token::T_TRUE);
    goto next;
  }

  if (token == "FALSE") {
    token_list->emplace_back(Token::T_TRUE);
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

  if (token == "WHERE") {
    token_list->emplace_back(Token::T_WHERE);
    goto next;
  }

  if (token == "GROUP") {
    token_list->emplace_back(Token::T_GROUP);
    goto next;
  }

  if (token == "ORDER") {
    token_list->emplace_back(Token::T_ORDER);
    goto next;
  }

  if (token == "BY") {
    token_list->emplace_back(Token::T_BY);
    goto next;
  }

  if (token == "HAVING") {
    token_list->emplace_back(Token::T_HAVING);
    goto next;
  }

  if (token == "AND") {
    token_list->emplace_back(Token::T_AND);
    goto next;
  }

  if (token == "OR") {
    token_list->emplace_back(Token::T_OR);
    goto next;
  }

  if (token == "LIMIT") {
    token_list->emplace_back(Token::T_LIMIT);
    goto next;
  }

  if (token == "OFFSET") {
    token_list->emplace_back(Token::T_OFFSET);
    goto next;
  }

  if (token == "CREATE") {
    token_list->emplace_back(Token::T_CREATE);
    goto next;
  }

  if (token == "WITH") {
    token_list->emplace_back(Token::T_WITH);
    goto next;
  }

  if (token == "BEGIN") {
    token_list->emplace_back(Token::T_BEGIN);
    goto next;
  }

  if (token == "MOD") {
    token_list->emplace_back(Token::T_MOD);
    goto next;
  }

  if (token == "DRAW") {
    token_list->emplace_back(Token::T_DRAW);
    goto next;
  }

  if (token == "TOP") {
    token_list->emplace_back(Token::T_TOP);
    goto next;
  }

  if (token == "RIGHT") {
    token_list->emplace_back(Token::T_RIGHT);
    goto next;
  }

  if (token == "BOTTOM") {
    token_list->emplace_back(Token::T_BOTTOM);
    goto next;
  }

  if (token == "LEFT") {
    token_list->emplace_back(Token::T_LEFT);
    goto next;
  }

  if (token == "IMPORT") {
    token_list->emplace_back(Token::T_IMPORT);
    goto next;
  }

  if (token == "TABLE") {
    token_list->emplace_back(Token::T_TABLE);
    goto next;
  }

  if (token == "CSV") {
    token_list->emplace_back(Token::T_CSV);
    goto next;
  }

  if (token == "HEADER") {
    token_list->emplace_back(Token::T_HEADER);
    goto next;
  }

  if (token == "AXIS") {
    token_list->emplace_back(Token::T_AXIS);
    goto next;
  }

  if (token == "BARCHART") {
    token_list->emplace_back(Token::T_BARCHART);
    goto next;
  }

  if (token == "LINECHART") {
    token_list->emplace_back(Token::T_LINECHART);
    goto next;
  }

  if (token == "AREACHART") {
    token_list->emplace_back(Token::T_AREACHART);
    goto next;
  }

  if (token == "ORIENTATION") {
    token_list->emplace_back(Token::T_ORIENTATION);
    goto next;
  }

  if (token == "HORIZONTAL") {
    token_list->emplace_back(Token::T_HORIZONTAL);
    goto next;
  }

  if (token == "VERTICAL") {
    token_list->emplace_back(Token::T_VERTICAL);
    goto next;
  }

  if (token == "STACKED") {
    token_list->emplace_back(Token::T_STACKED);
    goto next;
  }

  if (token == "ON") {
    token_list->emplace_back(Token::T_ON);
    goto next;
  }

  if (token == "OFF") {
    token_list->emplace_back(Token::T_OFF);
    goto next;
  }

  if (token == "XDOMAIN") {
    token_list->emplace_back(Token::T_XDOMAIN);
    goto next;
  }

  if (token == "YDOMAIN") {
    token_list->emplace_back(Token::T_YDOMAIN);
    goto next;
  }

  if (token == "ZDOMAIN") {
    token_list->emplace_back(Token::T_ZDOMAIN);
    goto next;
  }

  if (token == "LOGARITHMIC") {
    token_list->emplace_back(Token::T_LOGARITHMIC);
    goto next;
  }

  if (token == "INVERT") {
    token_list->emplace_back(Token::T_INVERT);
    goto next;
  }

  if (token == "TITLE") {
    token_list->emplace_back(Token::T_TITLE);
    goto next;
  }

  if (token == "SUBTITLE") {
    token_list->emplace_back(Token::T_SUBTITLE);
    goto next;
  }

  if (token == "GRID") {
    token_list->emplace_back(Token::T_GRID);
    goto next;
  }

  if (token == "LABELS") {
    token_list->emplace_back(Token::T_LABELS);
    goto next;
  }

  if (token == "INSIDE") {
    token_list->emplace_back(Token::T_INSIDE);
    goto next;
  }

  if (token == "OUTSIDE") {
    token_list->emplace_back(Token::T_OUTSIDE);
    goto next;
  }

  if (token == "ROTATE") {
    token_list->emplace_back(Token::T_ROTATE);
    goto next;
  }

  if (token == "LEGEND") {
    token_list->emplace_back(Token::T_LEGEND);
    goto next;
  }

  if (token == "<<") {
    token_list->emplace_back(Token::T_LSHIFT);
    goto next;
  }

  if (token == ">>") {
    token_list->emplace_back(Token::T_RSHIFT);
    goto next;
  }

  token_list->push_back(token);
  goto next; // poor mans tail recursion optimization
}

void tokenizeQuery(
    const std::string& query,
    std::vector<Token>* token_list) {
  const char* str = query.c_str();
  tokenizeQuery(&str, str + query.size(), token_list);
}

}
}
