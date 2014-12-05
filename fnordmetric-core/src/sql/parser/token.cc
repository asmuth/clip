/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/parser/token.h>
#include <fnord/base/exception.h>
#include <fnord/base/inspect.h>
#include <string.h>

namespace fnordmetric {
namespace query {

Token::Token(kTokenType token_type) :
    type_(token_type) {}

Token::Token(
    kTokenType token_type,
    const char* data,
    size_t len) :
    type_(token_type) {
  if (data != nullptr) {
    str_ = std::string(data, len);
  }
}

Token::Token(
    kTokenType token_type,
    const std::string& str) :
    type_(token_type),
    str_(str) {}

Token::Token(const Token& copy) :
    type_(copy.type_),
    str_(copy.str_) {}

void Token::debugPrint() const {
  if (str_.size() == 0) {
    printf("%s\n", getTypeName(type_));
  } else {
    printf("%s(%s)\n", getTypeName(type_), str_.c_str());
  }
}

const char* Token::getTypeName(kTokenType type) {
  switch (type) {
    case T_SELECT: return "T_SELECT";
    case T_FROM: return "T_FROM";
    case T_WHERE: return "T_WHERE";
    case T_GROUP: return "T_GROUP";
    case T_ORDER: return "T_ORDER";
    case T_BY: return "T_BY";
    case T_HAVING: return "T_HAVING";
    case T_LIMIT: return "T_LIMIT";
    case T_OFFSET: return "T_OFFSET";
    case T_ASC: return "T_ASC";
    case T_DESC: return "T_DESC";
    case T_COMMA: return "T_COMMA";
    case T_DOT: return "T_DOT";
    case T_IDENTIFIER: return "T_IDENTIFIER";
    case T_STRING: return "T_STRING";
    case T_NUMERIC: return "T_NUMERIC";
    case T_SEMICOLON: return "T_SEMICOLON";
    case T_LPAREN: return "T_LPAREN";
    case T_RPAREN: return "T_RPAREN";
    case T_AND: return "T_AND";
    case T_OR: return "T_OR";
    case T_EQUAL: return "T_EQUAL";
    case T_NEQUAL: return "T_NEQUAL";
    case T_PLUS: return "T_PLUS";
    case T_MINUS: return "T_MINUS";
    case T_ASTERISK: return "T_ASTERISK";
    case T_SLASH: return "T_SLASH";
    case T_AS: return "T_AS";
    case T_NOT: return "T_NOT";
    case T_TRUE: return "T_TRUE";
    case T_FALSE: return "T_FALSE";
    case T_BANG: return "T_BANG";
    case T_CIRCUMFLEX: return "T_CIRCUMFLEX";
    case T_TILDE: return "T_TILDE";
    case T_PERCENT: return "T_PERCENT";
    case T_DIV: return "T_DIV";
    case T_MOD: return "T_MOD";
    case T_AMPERSAND: return "T_AMPERSAND";
    case T_PIPE: return "T_PIPE";
    case T_LSHIFT: return "T_LSHIFT";
    case T_RSHIFT: return "T_RSHIFT";
    case T_LT: return "T_LT";
    case T_LTE: return "T_LTE";
    case T_GT: return "T_GT";
    case T_GTE: return "T_GTE";
    case T_BEGIN: return "T_BEGIN";
    case T_CREATE: return "T_CREATE";
    case T_WITH: return "T_WITH";
    case T_IMPORT: return "T_IMPORT";
    case T_TABLE: return "T_TABLE";
    case T_ON: return "T_ON";
    case T_OFF: return "T_OFF";
    case T_EOF: return "T_EOF";
    case T_DRAW: return "T_DRAW";
    case T_LINECHART: return "T_LINECHART";
    case T_AREACHART: return "T_AREACHART";
    case T_BARCHART: return "T_BARCHART";
    case T_POINTCHART: return "T_POINTCHART";
    case T_HEATMAP: return "T_HEATMAP";
    case T_HISTOGRAM: return "T_HISTOGRAM";
    case T_AXIS: return "T_AXIS";
    case T_TOP: return "T_TOP";
    case T_RIGHT: return "T_RIGHT";
    case T_BOTTOM: return "T_BOTTOM";
    case T_LEFT: return "T_LEFT";
    case T_ORIENTATION: return "T_ORIENTATION";
    case T_HORIZONTAL: return "T_HORIZONTAL";
    case T_VERTICAL: return "T_VERTICAL";
    case T_STACKED: return "T_STACKED";
    case T_XDOMAIN: return "T_XDOMAIN";
    case T_YDOMAIN: return "T_YDOMAIN";
    case T_ZDOMAIN: return "T_ZDOMAIN";
    case T_XGRID: return "T_XGRID";
    case T_YGRID: return "T_YGRID";
    case T_LOGARITHMIC: return "T_LOGARITHMIC";
    case T_INVERT: return "T_INVERT";
    case T_TITLE: return "T_TITLE";
    case T_SUBTITLE: return "T_SUBTITLE";
    case T_GRID: return "T_GRID";
    case T_LABELS: return "T_LABELS";
    case T_TICKS: return "T_TICKS";
    case T_INSIDE: return "T_INSIDE";
    case T_OUTSIDE: return "T_OUTSIDE";
    case T_ROTATE: return "T_ROTATE";
    case T_LEGEND: return "T_LEGEND";
    case T_OVER: return "T_OVER";
    case T_TIMEWINDOW: return "T_TIMEWINDOW";
    default: return "T_UNKNOWN_TOKEN";
  }
}

Token::kTokenType Token::getType() const {
  return type_;
}

bool Token::operator==(const std::string& string) const {
  if (str_.size() != string.size()) {
    return false;
  }

  return strncasecmp(str_.c_str(), string.c_str(), string.size()) == 0;
}

bool Token::operator==(kTokenType type) const {
  return type_ == type;
}

bool Token::operator==(const Token& other) const {
  if (type_ != other.type_) {
    return false;
  }

  return str_ == other.str_;
}

int64_t Token::getInteger() const {
  return std::stoi(str_); // FIXPAUL catch errors
}

const std::string Token::getString() const {
  return str_;
}

} // namespace query
} // namespace fnordmetric

namespace fnord {

template <>
std::string inspect<
    fnordmetric::query::Token::kTokenType>(
    const fnordmetric::query::Token::kTokenType& value) {
  return fnordmetric::query::Token::getTypeName(value);
}

template <>
std::string inspect<
    fnordmetric::query::Token>(
    const fnordmetric::query::Token& value) {
  return fnordmetric::query::Token::getTypeName(value.getType());
}

} // namespace fnord
