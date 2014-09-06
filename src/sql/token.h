/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_QUERY_TOKEN_H
#define _FNORDMETRIC_QUERY_TOKEN_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace query {

class Token {
public:
  enum kTokenType {
    T_SELECT,
    T_FROM,
    T_WHERE,
    T_GROUP,
    T_ORDER,
    T_BY,
    T_HAVING,
    T_LIMIT,
    T_OFFSET,
    T_ASC,
    T_DESC,
    T_COMMA,
    T_DOT,
    T_IDENTIFIER,
    T_STRING,
    T_NUMERIC,
    T_SEMICOLON,
    T_LPAREN,
    T_RPAREN,
    T_AND,
    T_OR,
    T_EQUAL,
    T_PLUS,
    T_MINUS,
    T_ASTERISK,
    T_SLASH,
    T_AS,
    T_NOT,
    T_TRUE,
    T_FALSE,
    T_BANG,
    T_CIRCUMFLEX,
    T_TILDE,
    T_PERCENT,
    T_DIV,
    T_MOD,
    T_AMPERSAND,
    T_PIPE,
    T_LSHIFT,
    T_RSHIFT,
    T_LT,
    T_GT,
    T_BEGIN,
    T_CREATE,
    T_WITH,
    T_IMPORT,
    T_TABLE,
    T_CSV,
    T_HEADER,
    T_ON,
    T_OFF,

    T_DRAW,
    T_LINECHART,
    T_AREACHART,
    T_BARCHART,
    T_POINTCHART,
    T_HEATMAP,
    T_HISTOGRAM,
    T_AXIS,
    T_TOP,
    T_RIGHT,
    T_BOTTOM,
    T_LEFT,
    T_ORIENTATION,
    T_HORIZONTAL,
    T_VERTICAL,
    T_STACKED,
    T_XDOMAIN,
    T_YDOMAIN,
    T_ZDOMAIN,
    T_XGRID,
    T_YGRID,
    T_LOGARITHMIC,
    T_INVERT,
    T_TITLE,
    T_SUBTITLE,
    T_GRID,
    T_LABELS,
    T_INSIDE,
    T_OUTSIDE,
    T_ROTATE,
    T_LEGEND
  };

  Token(kTokenType token_type);
  Token(kTokenType token_type, const char* data, size_t size);
  Token(const Token& copy);
  ~Token();
  Token& operator=(const Token& copy) = delete;
  bool operator==(const Token& other) const;
  bool operator==(const std::string& string) const;
  bool operator==(kTokenType type) const;
  kTokenType getType() const;
  static const char* getTypeName(kTokenType type);
  const std::string getString() const;
  int64_t getInteger() const; // FIXPAUL removeme

  void debugPrint() const;

protected:
  char* const data_;
  const size_t len_;
  const kTokenType type_;
};

}
}
#endif
